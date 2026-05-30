import os
import subprocess
import json
from flask import Flask, request, jsonify, send_file
import base64
from flask_cors import CORS
from werkzeug.utils import secure_filename

app = Flask(__name__)
CORS(app)

UPLOAD_FOLDER = os.path.join(os.path.dirname(__file__), 'uploads')
TESTCASES_FOLDER = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../testcases'))
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

# Path to the clone_detector binary
CLONE_DETECTOR_BIN = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../build/clone_detector'))

def _run_compile(cmd):
    """Run a compilation command; return True on success, False if binary missing."""
    try:
        subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        return True
    except FileNotFoundError:
        return False  # compiler not installed
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Compilation failed: {e.stderr.decode(errors='replace').strip()}")


def compile_to_llvm_ir(filepath):
    ext = os.path.splitext(filepath)[1].lower()
    ll_filepath = filepath + '.ll'

    if ext == '.c':
        cmd = ['clang', '-S', '-emit-llvm', '-O0',
               '-Xclang', '-disable-O0-optnone', filepath, '-o', ll_filepath]
        if not _run_compile(cmd):
            raise RuntimeError("clang not found. Install with: sudo apt install clang")

    elif ext in ['.cpp', '.cxx', '.cc']:
        cmd = ['clang++', '-S', '-emit-llvm', '-O0',
               '-Xclang', '-disable-O0-optnone', filepath, '-o', ll_filepath]
        if not _run_compile(cmd):
            raise RuntimeError("clang++ not found. Install with: sudo apt install clang")

    elif ext == '.rs':
        cmd = ['rustc', '--emit=llvm-ir', '--crate-type=lib',
               '-C', 'opt-level=0', '-C', 'debug-assertions=off',
               '-C', 'link-dead-code=yes', filepath, '-o', ll_filepath]
        if not _run_compile(cmd):
            raise RuntimeError("rustc not found. Install Rust from https://rustup.rs")

    elif ext in ['.f90', '.f']:
        # Try compilers in order: flang-new → flang → error
        fortran_tried = []

        for compiler in ['flang-new', 'flang']:
            cmd = [compiler, '-S', '-emit-llvm', '-O0', filepath, '-o', ll_filepath]
            fortran_tried.append(compiler)
            if _run_compile(cmd):
                break  # success
        else:
            raise RuntimeError(
                f"No Fortran compiler found (tried: {', '.join(fortran_tried)}). "
                f"Install with: sudo apt install flang   OR   sudo apt install llvm-18"
            )

    else:
        raise ValueError(f"Unsupported file extension: {ext}")


            
    # Clean up LLVM 19 specific attributes in Rust IR for compatibility with older LLVM parser
    if ext == '.rs' and os.path.exists(ll_filepath):
        try:
            import re
            with open(ll_filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            content = content.replace('nocreateundeforpoison', '')
            content = re.sub(r'captures\([^)]*\)', '', content)
            content = content.replace('inbounds nuw', 'inbounds')
            content = content.replace('trunc nuw', 'trunc')
            with open(ll_filepath, 'w', encoding='utf-8') as f:
                f.write(content)
        except Exception as strip_err:
            print(f"Warning: Failed to strip Rust LLVM attributes: {strip_err}")
            
    return ll_filepath

@app.route('/api/upload', methods=['POST'])
def upload_and_compare():
    if 'files' not in request.files:
        return jsonify({'error': 'No files part'}), 400
        
    files = request.files.getlist('files')
    if len(files) < 2:
        return jsonify({'error': 'Please upload at least 2 files for comparison.'}), 400
        
    saved_files = []
    ll_files = []
    
    for file in files:
        if file.filename == '':
            continue
        filename = secure_filename(file.filename)
        filepath = os.path.join(UPLOAD_FOLDER, filename)
        file.save(filepath)
        saved_files.append(filepath)
        
    try:
        # Compile all files to LLVM IR
        for fp in saved_files:
            ll_file = compile_to_llvm_ir(fp)
            ll_files.append(ll_file)
            
        # Run clone detector
        cmd = [CLONE_DETECTOR_BIN, '--multi'] + ll_files + ['--json']
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        
        # Parse JSON output
        comparisons = json.loads(result.stdout)
        
        # Map .ll paths back to original relative paths
        def get_rel_path(p):
            p = p.replace('.ll', '')
            if p.startswith(TESTCASES_FOLDER):
                return os.path.relpath(p, TESTCASES_FOLDER).replace('\\', '/')
            elif p.startswith(UPLOAD_FOLDER):
                return os.path.relpath(p, UPLOAD_FOLDER).replace('\\', '/')
            return os.path.basename(p)

        for cmp in comparisons:
            if 'file1' in cmp:
                cmp['file1'] = get_rel_path(cmp['file1'])
            if 'file2' in cmp:
                cmp['file2'] = get_rel_path(cmp['file2'])
        
        # Generate matrix data for frontend
        return jsonify({
            'success': True,
            'comparisons': comparisons,
            'files_processed': [get_rel_path(f) for f in saved_files]
        })
        
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/testcases', methods=['GET'])
def list_testcases():
    testcases = []
    for root, dirs, files in os.walk(TESTCASES_FOLDER):
        for file in files:
            if file.endswith(('.c', '.cpp', '.rs', '.f90', '.f')):
                rel_path = os.path.relpath(os.path.join(root, file), TESTCASES_FOLDER)
                # Convert path separators to forward slash for consistency
                testcases.append(rel_path.replace('\\', '/'))
    return jsonify({'testcases': testcases})

@app.route('/api/compare_testcases', methods=['POST'])
def compare_testcases():
    data = request.json
    selected_files = data.get('files', [])
    
    if len(selected_files) < 2:
        return jsonify({'error': 'Please select at least 2 files for comparison.'}), 400
        
    full_paths = [os.path.join(TESTCASES_FOLDER, f) for f in selected_files]
    
    ll_files = []
    try:
        for fp in full_paths:
            if not os.path.exists(fp):
                return jsonify({'error': f'File not found: {fp}'}), 404
            ll_file = compile_to_llvm_ir(fp)
            ll_files.append(ll_file)
            
        cmd = [CLONE_DETECTOR_BIN, '--multi'] + ll_files + ['--json']
        result = subprocess.run(cmd, capture_output=True, text=True, check=True)
        
        comparisons = json.loads(result.stdout)
        
        # Map .ll paths back to original relative paths
        def get_rel_path(p):
            p = p.replace('.ll', '')
            if p.startswith(TESTCASES_FOLDER):
                return os.path.relpath(p, TESTCASES_FOLDER).replace('\\', '/')
            elif p.startswith(UPLOAD_FOLDER):
                return os.path.relpath(p, UPLOAD_FOLDER).replace('\\', '/')
            return os.path.basename(p)

        for cmp in comparisons:
            if 'file1' in cmp:
                cmp['file1'] = get_rel_path(cmp['file1'])
            if 'file2' in cmp:
                cmp['file2'] = get_rel_path(cmp['file2'])
        
        return jsonify({
            'success': True,
            'comparisons': comparisons,
            'files_processed': [get_rel_path(f) for f in full_paths]
        })
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/testcase_content', methods=['GET'])
def testcase_content():
    filepath = request.args.get('file', '')
    if not filepath:
        return jsonify({'error': 'No file specified'}), 400

    full_path = os.path.join(TESTCASES_FOLDER, filepath)
    # Security: ensure the path is within TESTCASES_FOLDER
    full_path = os.path.realpath(full_path)
    if not full_path.startswith(os.path.realpath(TESTCASES_FOLDER)):
        return jsonify({'error': 'Invalid file path'}), 403

    if not os.path.exists(full_path):
        return jsonify({'error': f'File not found: {filepath}'}), 404

    try:
        with open(full_path, 'r') as f:
            content = f.read()
        return jsonify({'content': content, 'file': filepath})
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/generate_graph', methods=['POST'])
def generate_graph():
    data = request.json
    filename = data.get('file', '')
    func_name = data.get('function', '')
    graph_type = data.get('type', 'cfg')

    if not filename or not func_name:
        return jsonify({'error': 'File and function must be specified'}), 400

    # Determine file path
    full_path = os.path.join(TESTCASES_FOLDER, filename)
    if not os.path.exists(full_path):
        full_path = os.path.join(UPLOAD_FOLDER, filename)
        if not os.path.exists(full_path):
            return jsonify({'error': f'File not found: {filename}'}), 404

    try:
        ll_file = compile_to_llvm_ir(full_path)
        
        flag = '--cfg' if graph_type == 'cfg' else '--dfg'
        cmd = [CLONE_DETECTOR_BIN, flag, ll_file, func_name]
        
        # Run with cwd=build to ensure ../results/ goes to the right place
        build_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../build'))
        result = subprocess.run(cmd, capture_output=True, text=True, check=True, cwd=build_dir)
        
        output_image_rel = None
        for line in result.stdout.split('\n'):
            if line.startswith('GRAPH_GENERATED:'):
                output_image_rel = line.split('GRAPH_GENERATED:')[1].strip()
                break
                
        if not output_image_rel:
            return jsonify({'error': 'Graph generation failed or function not found.'}), 500
            
        # The output_image_rel is like ../results/cfg_func.png from build/
        output_image_abs = os.path.abspath(os.path.join(build_dir, output_image_rel))
        
        if not os.path.exists(output_image_abs):
            return jsonify({'error': 'Graph file was not created.'}), 500

        dot_file_abs = output_image_abs.replace('.png', '.dot')
        dot_text = ""
        if os.path.exists(dot_file_abs):
            with open(dot_file_abs, 'r') as f:
                dot_text = f.read()
                
        with open(output_image_abs, "rb") as f:
            image_data = f.read()
            image_base64 = base64.b64encode(image_data).decode('utf-8')

        return jsonify({
            'image': image_base64,
            'dot_text': dot_text,
            'metrics': result.stdout
        })
        
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)

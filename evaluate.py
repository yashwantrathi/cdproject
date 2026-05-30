import os
import subprocess
import json

TEST_DIR = 'testcases/benchmark'
CLONE_DETECTOR = 'build/clone_detector'

def compile_ll(file):
    if file.endswith('.ll'): return file
    ext = os.path.splitext(file)[1]
    ll_file = file + '.ll'
    if os.path.exists(ll_file): return ll_file
    
    cmd = []
    if ext == '.c': cmd = ['clang', '-S', '-emit-llvm', '-O0', file, '-o', ll_file]
    elif ext == '.cpp': cmd = ['clang++', '-S', '-emit-llvm', '-O0', file, '-o', ll_file]
    elif ext == '.rs': cmd = ['rustc', '--emit=llvm-ir', '--crate-type=lib', '-C', 'opt-level=0', file, '-o', ll_file]
    elif ext == '.f90': cmd = ['flang-new', '-S', '-emit-llvm', '-O0', file, '-o', ll_file]
    else: return None
    
    try:
        subprocess.run(cmd, check=True, stderr=subprocess.DEVNULL)
        if ext == '.rs':
            with open(ll_file, 'r') as f: content = f.read()
            content = content.replace('nocreateundeforpoison', '').replace('inbounds nuw', 'inbounds').replace('trunc nuw', 'trunc')
            import re
            content = re.sub(r'captures\([^)]*\)', '', content)
            with open(ll_file, 'w') as f: f.write(content)
        return ll_file
    except:
        return None

algorithms = ['sorting/bubble_sort', 'sorting/merge_sort', 'searching/binary_search', 'math/factorial']

print('Running evaluations...')
for alg in algorithms:
    files = []
    for ext in ['.c', '.cpp', '.rs', '.f90']:
        path = os.path.join(TEST_DIR, f'{alg}{ext}')
        if os.path.exists(path):
            ll = compile_ll(path)
            if ll: files.append(ll)
            
    if len(files) >= 2:
        res = subprocess.run([CLONE_DETECTOR, '--multi'] + files + ['--json'], capture_output=True, text=True)
        try:
            data = json.loads(res.stdout)
            print(f'\n--- {alg} Equivalent Implementations (True Positives) ---')
            for cmp in data:
                if cmp['similarity'] >= 65:
                    print(f"{os.path.basename(cmp['file1'])} vs {os.path.basename(cmp['file2'])}: {cmp['similarity']}% ({cmp['result']})")
                else:
                    print(f"[MISS] {os.path.basename(cmp['file1'])} vs {os.path.basename(cmp['file2'])}: {cmp['similarity']}%")
        except:
            print(f'Failed to parse {alg}')
            
# Cross algorithm to check false positives
print('\n--- Cross-Algorithm Check (False Positives) ---')
cross_files = [compile_ll(os.path.join(TEST_DIR, 'sorting/bubble_sort.c')), compile_ll(os.path.join(TEST_DIR, 'sorting/merge_sort.c'))]
res = subprocess.run([CLONE_DETECTOR, '--multi'] + cross_files + ['--json'], capture_output=True, text=True)
data = json.loads(res.stdout)
for cmp in data:
    print(f"{os.path.basename(cmp['file1'])} vs {os.path.basename(cmp['file2'])}: {cmp['similarity']}% ({cmp['result']})")

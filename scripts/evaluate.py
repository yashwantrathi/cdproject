import os
import json
import subprocess
from glob import glob

base_dir = os.path.dirname(os.path.abspath(__file__))
clone_detector_bin = os.path.join(base_dir, '../build/clone_detector')
benchmark_dir = os.path.join(base_dir, '../testcases/benchmark')

def compile_to_llvm_ir(filepath):
    ext = os.path.splitext(filepath)[1].lower()
    ll_filepath = os.path.splitext(filepath)[0] + '.ll'
    
    cmd = None
    if ext == '.c':
        cmd = ['clang', '-S', '-emit-llvm', '-O0', filepath, '-o', ll_filepath]
    elif ext in ['.cpp', '.cxx', '.cc']:
        cmd = ['clang++', '-S', '-emit-llvm', '-O0', filepath, '-o', ll_filepath]
    elif ext == '.rs':
        cmd = ['rustc', '--emit=llvm-ir', '-C', 'opt-level=0', filepath, '-o', ll_filepath]
    elif ext in ['.f90', '.f']:
        cmd = ['flang-new', '-S', '-emit-llvm', '-O0', filepath, '-o', ll_filepath]
        
    if not cmd:
        return None
        
    try:
        subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    except subprocess.CalledProcessError as e:
        if ext in ['.f90', '.f'] and 'flang-new' in cmd[0]:
            cmd[0] = 'flang'
            try:
                subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            except:
                return None
        else:
            return None
    except FileNotFoundError:
        print(f"Compiler for {ext} not found. Skipping {filepath}")
        return None
    return ll_filepath

def evaluate():
    # Discover all benchmark files
    files = glob(os.path.join(benchmark_dir, '**', '*.*'), recursive=True)
    source_files = [f for f in files if f.endswith(('.c', '.cpp', '.rs', '.f90'))]
    
    # Ground truth: files in the same folder (same algorithm) are clones.
    # Files in different folders are not clones.
    ll_files = []
    print("Compiling benchmark files to LLVM IR...")
    for f in source_files:
        ll = compile_to_llvm_ir(f)
        if ll:
            ll_files.append(ll)
            
    if len(ll_files) < 2:
        print("Not enough compiled files to evaluate.")
        return

    print("Running clone detector...")
    cmd = [clone_detector_bin, '--multi'] + ll_files + ['--json']
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    if result.returncode != 0:
        print("Error running clone detector:", result.stderr)
        return
        
    try:
        comparisons = json.loads(result.stdout)
    except json.JSONDecodeError:
        print("Failed to decode JSON output from clone detector.")
        return

    true_positives = 0
    false_positives = 0
    true_negatives = 0
    false_negatives = 0

    for comp in comparisons:
        f1 = comp['file1']
        f2 = comp['file2']
        
        # Determine ground truth
        dir1 = os.path.dirname(f1)
        dir2 = os.path.dirname(f2)
        is_actual_clone = (dir1 == dir2)
        
        is_predicted_clone = (comp['result'] == 'Strong Clone')
        
        if is_actual_clone and is_predicted_clone:
            true_positives += 1
        elif not is_actual_clone and is_predicted_clone:
            false_positives += 1
        elif is_actual_clone and not is_predicted_clone:
            false_negatives += 1
        elif not is_actual_clone and not is_predicted_clone:
            true_negatives += 1

    total = true_positives + false_positives + true_negatives + false_negatives
    
    precision = true_positives / (true_positives + false_positives) if (true_positives + false_positives) > 0 else 0
    recall = true_positives / (true_positives + false_negatives) if (true_positives + false_negatives) > 0 else 0
    f1_score = 2 * (precision * recall) / (precision + recall) if (precision + recall) > 0 else 0
    accuracy = (true_positives + true_negatives) / total if total > 0 else 0
    
    print("\n========== EVALUATION RESULTS ==========")
    print(f"Total Comparisons: {total}")
    print(f"True Positives: {true_positives}")
    print(f"False Positives: {false_positives}")
    print(f"True Negatives: {true_negatives}")
    print(f"False Negatives: {false_negatives}")
    print("----------------------------------------")
    print(f"Accuracy:  {accuracy * 100:.2f}%")
    print(f"Precision: {precision * 100:.2f}%")
    print(f"Recall:    {recall * 100:.2f}%")
    print(f"F1-Score:  {f1_score * 100:.2f}%")
    print("========================================")

if __name__ == '__main__':
    evaluate()

# EVALUATION DOCUMENT

# LLVM Clone Detection System

---

# 1. Introduction

This document evaluates the effectiveness of the LLVM Clone Detection System using multiple LLVM IR testcases.

The evaluation focuses on:
- clone detection accuracy
- similarity scoring
- CFG extraction
- visualization outputs
- analytics generation

---

# 2. Test Environment

## Platform

- Ubuntu (WSL)
- VS Code
- LLVM 18
- CMake
- Graphviz
- Python 3

---

# 3. Testcases Used

The following testcases were used for evaluation:

| File | Description |
|---|---|
| sample.ll | Basic add function |
| test2.ll | Similar add implementation |
| test3.ll | Different function logic |
| test4.ll | Loop-based summation |

---

# 4. Evaluation Methodology

The evaluation process:
1. Generate LLVM IR
2. Parse LLVM IR
3. Extract function fingerprints
4. Normalize opcodes
5. Compare fingerprints
6. Calculate similarity score
7. Generate visual analytics

Clone detection threshold:
- 70%

---

# 5. Clone Detection Results

| Function Pair | Similarity | Result |
|---|---|---|
| add vs sum | 100% | Clone Detected |
| add vs multiply | 50% | Partial Similarity |
| loopSum vs add | 0% | No Clone Detected |
| sum vs add | 100% | Clone Detected |

---

# 6. Similarity Analysis

## High Similarity Functions

Functions implementing identical logic produced:
- 100% similarity

Example:
- add vs sum

Reason:
- identical opcode structure
- identical computation pattern

---

## Partial Similarity Functions

Functions with partially matching operations produced:
- 50% similarity

Example:
- add vs multiply

Reason:
- similar structure
- different arithmetic behavior

---

## Non-Clone Functions

Different algorithmic logic produced:
- 0% similarity

Example:
- loopSum vs add

Reason:
- different control flow
- different opcode sequence
- loop-based structure

---

# 7. CFG Evaluation

The CFG Builder successfully extracted:
- basic blocks
- control flow edges
- branching structure

Generated outputs:
- DOT graph files
- PNG CFG visualizations

Example outputs:
- cfg_loopSum.png
- cfg_main.png

The CFG graphs correctly represented:
- loops
- branch transitions
- control flow structure

---

# 8. Visualization Evaluation

The visualization pipeline successfully generated:
- similarity heatmaps
- similarity bar graphs

Libraries used:
- Matplotlib
- Seaborn

Generated files:
- similarity_heatmap.png
- similarity_bar.png

Observations:
- clones clearly visible as high similarity regions
- non-clones visible as low similarity regions

---

# 9. Batch Mode Evaluation

Batch mode successfully:
- scanned testcase directory
- compared multiple LLVM IR files
- generated analytics automatically

Advantages observed:
- faster evaluation
- automated testing
- scalable comparison pipeline

---

# 10. Performance Observations

Observed characteristics:
- fast IR parsing
- lightweight fingerprint generation
- efficient opcode comparison
- low memory overhead

The modular design simplified debugging and extension.

---

# 11. Limitations Observed

Current limitations:
- simple similarity algorithm
- no semantic graph matching
- limited optimization-aware analysis
- LLVM version compatibility issues

The system currently focuses primarily on opcode-level comparison.

---

# 12. Future Evaluation Improvements

Future evaluation metrics may include:
- precision
- recall
- false positive rate
- false negative rate
- graph similarity scoring
- ML-based evaluation

---

# 13. Conclusion

The evaluation demonstrates that the LLVM Clone Detection System can successfully identify structurally similar functions using LLVM IR analysis.

The generated CFGs and analytics visualizations further validate the correctness and usefulness of the detection pipeline.

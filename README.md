# LLVM Clone Detection System

## Overview

LLVM Clone Detection System is a compiler-based cross-language code clone detection framework that analyzes LLVM Intermediate Representation (IR) to identify semantically similar functions across source programs.

The project performs:
- LLVM IR parsing
- Function fingerprinting
- CFG extraction
- Similarity analysis
- Clone detection
- Visualization and analytics

---

# Features

- LLVM IR Parsing
- Function Fingerprinting
- Opcode Normalization
- CFG (Control Flow Graph) Extraction
- CFG Graph Visualization
- Similarity Scoring
- Clone Detection
- Batch Testing
- CSV Export
- Heatmap Visualization
- Similarity Analytics Graphs
- Threshold Configuration
- Verbose Debug Mode

---

# Tech Stack

- C++
- LLVM 18
- CMake
- Graphviz
- Python
- Pandas
- Matplotlib
- Seaborn

---

# Project Structure

```bash
clone-detector/
│
├── include/
│   ├── ir_parser/
│   ├── fingerprint/
│   ├── similarity/
│   ├── normalizer/
│   └── cfg_builder/
│
├── src/
│   ├── ir_parser/
│   ├── fingerprint/
│   ├── similarity/
│   ├── normalizer/
│   └── cfg_builder/
│
├── testcases/
│
├── results/
│
├── build/
│
├── visualize.py
│
├── README.md
└── CMakeLists.txt
```

---

# Installation

## Install LLVM

```bash
sudo apt update
sudo apt install llvm clang cmake -y
```

## Install Graphviz

```bash
sudo apt install graphviz -y
```

## Install Python Libraries

```bash
pip install pandas matplotlib seaborn
```

---

# Build Instructions

```bash
git clone https://github.com/yashwantrathi/cdproject.git

cd cdproject

mkdir build
cd build

cmake ..
make
```

---

# Generate LLVM IR

## C Program

```bash
clang -S -emit-llvm test.c -o test.ll
```

## C++ Program

```bash
clang++ -S -emit-llvm test.cpp -o test.ll
```

---

# Usage

## Compare Two LLVM IR Files

```bash
./clone_detector file1.ll file2.ll
```

Example:

```bash
./clone_detector ../testcases/sample.ll ../testcases/test2.ll
```

---

# Verbose Mode

```bash
./clone_detector file1.ll file2.ll --verbose
```

---

# Custom Similarity Threshold

```bash
./clone_detector file1.ll file2.ll 90
```

---

# Batch Testing

```bash
./clone_detector --batch
```

---

# Outputs

Generated outputs:

- results/output.txt
- results/results.csv
- CFG graph images
- similarity_heatmap.png
- similarity_bar.png

---

# Visualization

Generate analytics graphs:

```bash
python visualize.py
```

This generates:
- Similarity Heatmap
- Similarity Bar Graph

---

# CFG Visualization

CFG graphs are automatically generated in:

```bash
results/
```

Example:
- cfg_add.png
- cfg_loopSum.png

---

# Example Results

## Clone Detection

| Function Pair | Similarity | Result |
|---|---|---|
| add vs sum | 100% | Clone |
| add vs multiply | 50% | Partial Similarity |
| loopSum vs add | 0% | No Clone |

---

# Future Scope

- Rust Support
- Data Flow Graph Analysis
- AST-based Clone Detection
- GUI/Web Dashboard
- ML-based Similarity Detection
- Advanced Graph Similarity

---

# Authors

Yashwanth Rathi

---

# License

Academic Project

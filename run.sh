#!/bin/bash
# ==============================================================================
# Run Script for LLVM Clone Detection System (Evaluation Mode)
# ==============================================================================

echo "================================================="
echo " IRClone - Batch Evaluation Pipeline"
echo "================================================="

if [ ! -f "build/clone_detector" ]; then
    echo "Error: clone_detector executable not found."
    echo "Please run ./build.sh first."
    exit 1
fi

# Ensure directories exist
mkdir -p results
mkdir -p results/cfg_graphs
mkdir -p results/dfg_graphs

cd build || exit 1

# Initialize output CSV
echo "Function1,Function2,Similarity,Clone Result" > ../results/results.csv

echo "Running Baseline Evaluation (>= 5 Test Cases)..."

echo "[1/5] Testing C vs C++ (Expected: Strong Clone)"
./clone_detector ../testcases/benchmark/sorting/bubble_sort.c.ll ../testcases/benchmark/sorting/bubble_sort.cpp.ll

echo "[2/5] Testing C vs Fortran (Expected: Strong Clone)"
./clone_detector ../testcases/benchmark/sorting/bubble_sort.c.ll ../testcases/benchmark/sorting/bubble_sort.f90.ll

echo "[3/5] Testing C vs Rust (Expected: Partial / Strong Clone)"
./clone_detector ../testcases/benchmark/sorting/bubble_sort.c.ll ../testcases/benchmark/sorting/bubble_sort.rs.ll

echo "[4/5] Testing Different Algorithms: Bubble vs Merge (Expected: Different / False Positive Test)"
./clone_detector ../testcases/benchmark/sorting/bubble_sort.c.ll ../testcases/benchmark/sorting/merge_sort.c.ll

echo "[5/5] Testing Searching Algorithms: C vs C++ (Expected: Strong Clone)"
./clone_detector ../testcases/benchmark/searching/binary_search.c.ll ../testcases/benchmark/searching/binary_search.cpp.ll

cd ..

echo "================================================="
echo "Generating Visualizations (Heatmap & Bar Charts)..."
python3 visualize.py

echo "Batch Evaluation Complete!"
echo "Check the 'results/' folder for CSV metrics and PNG graphs."

#!/bin/bash

echo "================================="
echo " LLVM Clone Detection Batch Run "
echo "================================="

cd build

# Clear previous CSV
echo "Function1,Function2,Similarity,Clone" > ../results/results.csv

# Strong clones
./clone_detector ../testcases/add.ll ../testcases/sum.ll

./clone_detector ../testcases/bubble1.ll ../testcases/bubble2.ll

# Cross-language strong clones
./clone_detector ../testcases/add.ll ../testcases/fort_add.ll
./clone_detector ../testcases/bubble.ll ../testcases/fort_bubble.ll

# Rust cross-language
echo ""
echo "--- Rust Comparisons ---"
./clone_detector ../testcases/add.ll ../testcases/rust_add.ll
./clone_detector ../testcases/bubble.ll ../testcases/rustbubble.ll | grep "bubbleSort" -A 4
./clone_detector ../testcases/selc.ll ../testcases/rust_selc.ll | grep "selectionSort" -A 4
echo "------------------------"

# Partial similarity
./clone_detector ../testcases/bubble.ll ../testcases/selc.ll

# Different logic
./clone_detector ../testcases/bubble.ll ../testcases/merge.ll
./clone_detector ../testcases/add.ll ../testcases/fort_bubble.ll

cd ..

echo ""
echo "Generating Visualizations..."

python3 visualize.py

echo ""
echo "Batch Evaluation Complete!"

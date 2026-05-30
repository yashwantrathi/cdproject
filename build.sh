#!/bin/bash
# ==============================================================================
# Build Script for LLVM Clone Detection System
# ==============================================================================

echo "================================================="
echo " Building IRClone - LLVM Clone Detection Engine "
echo "================================================="

# Create build directory if it doesn't exist
mkdir -p build
cd build || exit 1

# Generate build files
echo "[1/3] Configuring CMake..."
cmake ..

# Compile the C++ LLVM engine
echo "[2/3] Compiling C++ Backend..."
make -j$(nproc)

echo "[3/3] Build Successful."
echo "Executable located at: ./build/clone_detector"
echo "Run ./run.sh to execute evaluations."

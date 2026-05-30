#!/bin/bash

echo "==========================================="
echo " Cross-Language Clone Detection Setup & Run"
echo " Languages: C, C++, Fortran, Rust"
echo "==========================================="

mkdir -p testcases
cd testcases

# 1. Create C Test Case (Bubble Sort)
cat << 'EOF' > bubble.c
void bubbleSort(int arr[], int n) {
    int i, j, temp;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}
EOF

# 2. Create C++ Test Case (Bubble Sort)
cat << 'EOF' > bubble2.cpp
void sortArray(int* arr, int n) {
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}
EOF

# 3. Create Fortran Test Case (Bubble Sort)
cat << 'EOF' > fort_bubble.f90
subroutine bubbleSort(arr, n)
    implicit none
    integer, intent(inout) :: arr(n)
    integer, intent(in) :: n
    integer :: i, j, temp
    do i = 1, n-1
        do j = 1, n-i
            if (arr(j) > arr(j+1)) then
                temp = arr(j)
                arr(j) = arr(j+1)
                arr(j+1) = temp
            end if
        end do
    end do
end subroutine bubbleSort
EOF

# 4. Create Rust Test Case (Bubble Sort)
cat << 'EOF' > rustbubble.rs
#[no_mangle]
pub extern "C" fn bubbleSort(arr: &mut [i32]) {
    let n = arr.len();
    for i in 0..n {
        for j in 0..(n - 1 - i) {
            if arr[j] > arr[j + 1] {
                arr.swap(j, j + 1);
            }
        }
    }
}
EOF

echo "[*] Compiling all languages to LLVM IR..."

# Compile C
clang -S -emit-llvm -O0 bubble.c -o bubble.ll

# Compile C++
clang++ -S -emit-llvm -O0 bubble2.cpp -o bubble2.ll

# Compile Fortran
flang-new -S -emit-llvm -O0 fort_bubble.f90 -o fort_bubble.ll

# Compile Rust 
# -C debug-assertions=off prevents bounds checking panics from cluttering the structural similarity graph
rustc --emit=llvm-ir --crate-type=lib -C debug-assertions=off rustbubble.rs
# Rust LLVM 19 attributes strip (to ensure compatibility with LLVM 18 parser)
sed -i 's/nocreateundeforpoison//g; s/captures([^)]*)//g; s/inbounds nuw/inbounds/g; s/trunc nuw/trunc/g' rustbubble.ll

cd ../build
echo "[*] Running Clone Detector across all 4 languages..."
echo ""

echo "--- C vs C++ ---"
./clone_detector ../testcases/bubble.ll ../testcases/bubble2.ll | grep -E "Similarity|Strong|Partial|Different"

echo ""
echo "--- C vs Fortran ---"
./clone_detector ../testcases/bubble.ll ../testcases/fort_bubble.ll | grep -E "Similarity|Strong|Partial|Different"

echo ""
echo "--- C vs Rust ---"
./clone_detector ../testcases/bubble.ll ../testcases/rustbubble.ll | grep -E "Similarity|Strong|Partial|Different" | tail -n 2

echo ""
echo "All comparisons complete!"

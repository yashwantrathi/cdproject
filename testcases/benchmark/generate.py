import os

datasets = {
    "sorting/bubble_sort": {
        "c": """#include <stdio.h>
void bubble_sort(int arr[], int n) {
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
int main() { return 0; }
""",
        "cpp": """#include <iostream>
#include <vector>
void bubble_sort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                std::swap(arr[j], arr[j+1]);
            }
        }
    }
}
int main() { return 0; }
""",
        "rs": """fn bubble_sort(arr: &mut [i32]) {
    let n = arr.len();
    for i in 0..n {
        for j in 0..n-i-1 {
            if arr[j] > arr[j+1] {
                arr.swap(j, j+1);
            }
        }
    }
}
fn main() {}
""",
        "f90": """subroutine bubble_sort(arr, n)
    integer, intent(inout) :: arr(:)
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
end subroutine bubble_sort
program main
end program main
"""
    },
    "math/factorial": {
        "c": """#include <stdio.h>
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
int main() { return 0; }
""",
        "cpp": """#include <iostream>
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}
int main() { return 0; }
""",
        "rs": """fn factorial(n: i32) -> i32 {
    if n <= 1 {
        return 1;
    }
    n * factorial(n - 1)
}
fn main() {}
""",
        "f90": """recursive function factorial(n) result(res)
    integer, intent(in) :: n
    integer :: res
    if (n <= 1) then
        res = 1
    else
        res = n * factorial(n - 1)
    end if
end function factorial
program main
end program main
"""
    }
}

base_dir = os.path.dirname(os.path.abspath(__file__))

for path, implementations in datasets.items():
    folder = os.path.join(base_dir, os.path.dirname(path))
    os.makedirs(folder, exist_ok=True)
    basename = os.path.basename(path)
    
    for ext, code in implementations.items():
        filepath = os.path.join(folder, f"{basename}.{ext}")
        with open(filepath, "w") as f:
            f.write(code)

print("Benchmark datasets created.")

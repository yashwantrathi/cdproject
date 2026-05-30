#include <iostream>
#include <vector>

void matrix_multiply(const std::vector<std::vector<int>>& A,
                     const std::vector<std::vector<int>>& B,
                     std::vector<std::vector<int>>& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() { return 0; }

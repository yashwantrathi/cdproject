#include <iostream>
#include <vector>

void merge(std::vector<int>& arr, int l, int m, int r) {
    std::vector<int> L(arr.begin() + l, arr.begin() + m + 1);
    std::vector<int> R(arr.begin() + m + 1, arr.begin() + r + 1);
    int i = 0, j = 0, k = l;
    while (i < (int)L.size() && j < (int)R.size()) {
        if (L[i] <= R[j]) { arr[k] = L[i]; i++; }
        else { arr[k] = R[j]; j++; }
        k++;
    }
    while (i < (int)L.size()) { arr[k] = L[i]; i++; k++; }
    while (j < (int)R.size()) { arr[k] = R[j]; j++; k++; }
}

void merge_sort(std::vector<int>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort(arr, l, m);
        merge_sort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

int main() { return 0; }

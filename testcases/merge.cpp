void mergeSort(int arr[], int left, int right) {

    if(left >= right)
        return;

    int mid = left + (right - left) / 2;

    mergeSort(arr, left, mid);

    mergeSort(arr, mid + 1, right);
}

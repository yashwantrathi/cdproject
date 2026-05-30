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

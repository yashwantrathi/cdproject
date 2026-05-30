fn binary_search(arr: &[i32], target: i32) -> i32 {
    let mut low: i32 = 0;
    let mut high: i32 = arr.len() as i32 - 1;
    while low <= high {
        let mid = low + (high - low) / 2;
        if arr[mid as usize] == target {
            return mid;
        } else if arr[mid as usize] < target {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    -1
}

fn main() {}

#![no_std]

#[no_mangle]
pub extern "C" fn bubble_sort(arr: *mut i32) {

    unsafe {

        let a = *arr;
        let b = *arr.add(1);

        if a > b {

            *arr = b;
            *arr.add(1) = a;
        }
    }
}

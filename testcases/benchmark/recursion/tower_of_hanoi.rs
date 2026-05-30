static mut MOVE_COUNT: i32 = 0;

fn tower_of_hanoi(n: i32, _from: char, _to: char, _aux: char) {
    unsafe {
        if n == 1 {
            MOVE_COUNT += 1;
            return;
        }
        tower_of_hanoi(n - 1, _from, _aux, _to);
        MOVE_COUNT += 1;
        tower_of_hanoi(n - 1, _aux, _to, _from);
    }
}

fn main() {}

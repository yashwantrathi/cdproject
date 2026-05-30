fn power(base: i64, exp: i32) -> i64 {
    if exp == 0 { return 1; }
    if exp % 2 == 0 {
        let half = power(base, exp / 2);
        return half * half;
    }
    base * power(base, exp - 1)
}

fn main() {}

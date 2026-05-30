fn matrix_multiply(a: &Vec<Vec<i32>>, b: &Vec<Vec<i32>>, n: usize) -> Vec<Vec<i32>> {
    let mut c = vec![vec![0i32; n]; n];
    for i in 0..n {
        for j in 0..n {
            for k in 0..n {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    c
}

fn main() {}

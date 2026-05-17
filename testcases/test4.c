#include <stdio.h>

int loopSum(int n) {

    int sum = 0;

    for(int i=0;i<n;i++) {
        sum += i;
    }

    return sum;
}

int main() {
    printf("%d\n", loopSum(10));
    return 0;
}

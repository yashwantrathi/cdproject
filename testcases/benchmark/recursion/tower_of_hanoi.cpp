#include <iostream>

int move_count = 0;

void tower_of_hanoi(int n, char from, char to, char aux) {
    if (n == 1) {
        move_count++;
        return;
    }
    tower_of_hanoi(n - 1, from, aux, to);
    move_count++;
    tower_of_hanoi(n - 1, aux, to, from);
}

int main() { return 0; }

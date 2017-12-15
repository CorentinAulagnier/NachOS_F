#include "syscall.h"
void print(int n) {
    char a = 'c';
    int i;
    for (i = 0; i < n; i++) {
        GetChar(a);
        PutChar(a);
    }

}

int main() {
    print(2);
    Halt();
}

#include "syscall.h"

void print(int n) {
    int a = 0;
    int i;
    for (i = 0; i < n; i++) {
        GetInt(&a);
        PutInt(a);
    }

}

int main() {
    print(2);
    Halt();
}

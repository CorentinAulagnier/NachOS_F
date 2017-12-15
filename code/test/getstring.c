#include "syscall.h"

void print(int n) {
    char a[4] = "coco"; 
    int i;
    for (i = 0; i < n; i++) {
        GetString(a, 4);
        PutString(a);
    }

}

int main() {
    print(2);
    Halt();
}

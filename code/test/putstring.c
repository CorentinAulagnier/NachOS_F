#include "syscall.h"

void print(char* c) {
    PutString(c);
    PutString("\n");
}

int main() {
    print("abc");
    print("defgh");
    print("coucoucoucou");
    //Halt();
    return 0;
}

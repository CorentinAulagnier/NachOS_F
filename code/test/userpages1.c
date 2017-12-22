#include "syscall.h"
#define THIS "aaa"
#define THAT "bbb"

const int N = 10; // Choose it large enough!

void puts(char *s) {
    int i = 0;
    for (i = 0; s[i] != '\0'; i++) PutChar(s[i]);
}

void f(void *s) {
    int i;
    for (i = 0; i < N; i++)
        puts((char*)s);
}

int main() {
    UserThreadCreate(f, (void *) THIS);
    f((void*) THAT);
    Halt();
}

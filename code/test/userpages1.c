#include "syscall.h"
#define THIS "a"
#define THAT "b"

//const int N = 5; // Choose it large enough!

void puts(char *s) {
    int i = 0;
    for (i = 0; s[i] != '\0'; i++) PutChar(s[i]);
}

void f(void *s) {
    int i;
    for (i = 0; i < 5; i++)
        puts((char*)s);
}

int main() {
   //int d = UserThreadCreate(f, (void *) THIS);
    UserThreadCreate(f, (void *) THIS);
    f((void*) THAT);

   //UserThreadJoin(d);
    return 1;
    //Halt();
}

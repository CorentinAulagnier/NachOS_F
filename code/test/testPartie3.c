#include "syscall.h"

void f1() {
    PutChar('A');
}

void f2(int i){
    PutInt(i);
}

int main() {

    int i = 2;

    UserThreadCreate(&f1, NULL);
    UserThreadCreate(&f2, &i);


    UserThreadExit();
    UserThreadExit();

    //Halt();
}

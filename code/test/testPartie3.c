#include "syscall.h"

void f1(void* c) {

    PutChar(*(char*)c);
}

void f2(void* i){
//PutString("Bonjour\n");
    PutInt(*(int*)i);
}

int main() {

    int i = 45;
    //char c = 'A';
    //UserThreadCreate(&f1, &c);
    UserThreadCreate(&f2, &i);


    UserThreadExit();
    //UserThreadExit();

    Halt();
}

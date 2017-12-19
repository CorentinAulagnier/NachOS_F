#include "syscall.h"

void f1(void* c) {

    PutChar(*(char*)c);
    UserThreadExit();
}

void f2(void* i){
//PutString("Bonjour\n");
    PutInt(*(int*)i);
    UserThreadExit();
}

int main() {

    int i = 45;
    char c = 'A';
    int tid2 = UserThreadCreate(&f1, &c);
    int tid = UserThreadCreate(&f2, &i);

PutString("\n tid2 = ");
PutInt(tid2);
PutString("\n tid = ");
PutInt(tid);

    Halt();
}

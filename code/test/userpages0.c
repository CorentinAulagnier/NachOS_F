#include "syscall.h"

void f1(void* c) {
    int i=0;
    char ch = *(char*)c;
    while(i<10) {
        PutChar(ch);
        i++;
    }
    UserThreadExit();
}

void f2(void* c) {

    int t3 = UserThreadCreate(&f1, c);
    UserThreadJoin(t3);

    UserThreadExit();
}

int main() {
    char c1 = 'A', c2 = 'B';
    int t1 = UserThreadCreate(&f1, &c1);
    int t2 = UserThreadCreate(&f1, &c2);

    UserThreadJoin(t1);
    UserThreadJoin(t2);

	return 1;

}

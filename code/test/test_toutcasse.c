#include "syscall.h"

void f1(void* j) {
    if(*(int*)j>=1000){
        PutString("j = 1000\n");
        return;
    }
    int z = *(int*)j + 1;
    PutInt(z);
    PutString("\n");
    f1((void*)&z);
}

int
main(){
    int i = 0;
    PutString("START test_toocasse\n");

    UserThreadCreate(&f1, &i);

    PutString("END test_toocasse\n");

	return 1;
}

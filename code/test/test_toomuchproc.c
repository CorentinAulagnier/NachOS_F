#include "syscall.h"

int
main(){
    int i;
    PutString("START test_toomuchproc\n");
    for(i = 0;i<3;i++){
        ForkExec("userpages0");
    }
    PutString("END test_toomuchproc\n");

	return 1;
}

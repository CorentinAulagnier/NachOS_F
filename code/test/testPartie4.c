#include "syscall.h"

int
main(){
    PutString("START testPartie4\n");
    //ForkExec("userpages0");
    ForkExec("userpages1");
    PutString("END testPartie4\n");

	return 1;
}

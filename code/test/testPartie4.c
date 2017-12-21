#include "syscall.h"

main(){
    PutString("START testPartie4\n");
    ForkExec("../test/userpages0");
    ForkExec("../test/userpages1");
    PutString("END testPartie4\n");
}

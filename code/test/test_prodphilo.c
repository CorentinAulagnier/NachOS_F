#include "syscall.h"

int
main(){
    PutString("START test_prodphilo\n");
    ForkExec("testPartie3Philosophes");
    ForkExec("testPartie3ProducerConsumer");
    PutString("END test_prodphilo\n");

	return 1;
}

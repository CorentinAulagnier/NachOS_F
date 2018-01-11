#include "thread.h"
#include "system.h"
#include "addrspace.h"


extern int do_UserThreadCreate(int f, int arg, int fct_fin);
extern void do_UserThreadExit();
extern int do_UserThreadJoin(int tid);
extern void printList(SynchListThread* synchlist);

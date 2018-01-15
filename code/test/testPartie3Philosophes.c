#include "syscall.h"

/* Find here :
 * https://wwwhome.ewi.utwente.nl/~pieter/CS-OS/Philosophers.c
 */
 
#define N 5 /* Number of times each philosopher tries to eat */
#define P 5 /* Number of philosophers */

void* Print;
void* fork[P];
int Switch = 1;

void tphilosopher(void *ptr) {
    int i, k = *((int *) ptr);
    
    UserSemWait(Print);
    PutString("   ");
    PutInt(k);
    PutString("   : Pense\n");
    UserSemPost(Print);

    for(i = 1; i <= N; i++) {


        
        UserSemWait(fork[k]) ;
        UserSemWait(fork[(k+1) % P]) ;
                
        UserSemWait(Print);
        PutString("    ");
        PutInt(k);
        PutString("  : Mange\n");
        UserSemPost(Print);

        UserSemWait(Print);
        UserSemPost(fork[k]) ;
        UserSemPost(fork[(k+1) % P]) ;
        PutString("  ");
        PutInt(k);
        PutString("    : Rend les baguettes\n");
        UserSemPost(Print);
        
    }
    
    UserSemWait(Print);
    PutString("");
    PutInt(k);
    PutString("      : Quitte la table\n");
    UserSemPost(Print);
        
    UserThreadExit();
}

int main(int argc, char * argv[]) {

    PutString("\nThread : ACTION\n********************\n");
    
    int i, targ[P];
    int thread[P];
    
    Print = UserSemCreate("Sem Print", 1);

    for(i=0;i<P;i++) {
        fork[i] = UserSemCreate("Sem fork", 1);    
    }
    for(i=0;i<P;i++) {
        targ[i] = i;
        thread[i] = UserThreadCreate(&tphilosopher, &targ[i]);
    }
    for(i=0;i<P;i++) {
        UserThreadJoin(thread[i]);
        UserThreadExit();
    }
    return 0;
}


#include "syscall.h"

#define BUFFER_SIZE 5
#define MAX_PROD 20

void* Print;

void* empty ;
void* full;
void* mutex ;

int buffer[BUFFER_SIZE];;

int in = 0;
int out = 0;
int nextProduced = 0;

void producer (void *ignored) {
    int i;
    for (i=0; i<MAX_PROD; i++) {
        UserSemWait(empty);
        UserSemWait(mutex);
        
        buffer [in] = nextProduced;
        
        UserSemWait(Print);
        PutString("Produced : ");
        PutInt(nextProduced);
        PutString(" \n");
        UserSemPost(Print);
        
        nextProduced++;
        in = (in + 1) % BUFFER_SIZE;
        
        UserSemPost(mutex);
        UserSemPost(full);

    }
    UserThreadExit();
}

void consumer (void *ignored) {
    int i;
    for (i=0; i<MAX_PROD; i++) {
        UserSemWait(full);
        UserSemWait(mutex);
        
        int nextConsumed = buffer[out];
        out = (out + 1) % BUFFER_SIZE;

        UserSemWait(Print);
        PutString(" Consumed : ");
        PutInt(nextConsumed);
        PutString(" \n");
        UserSemPost(Print);
        
        UserSemPost(mutex);
        UserSemPost(empty);
    }
    UserThreadExit();
}



int main() {

    Print = UserSemCreate("Sem Print", 1);
    
    empty = UserSemCreate("Semaphore empty", BUFFER_SIZE);
    full = UserSemCreate("Semaphore full", 0);
    mutex = UserSemCreate("Semaphore mutex", 1);

    char* c = "Bonjour";
    
    int tidproducer = UserThreadCreate(&producer, &c);
    int tidconsumer = UserThreadCreate(&consumer, &c);

    UserThreadJoin(tidproducer);
    UserThreadJoin(tidconsumer);
    
    Halt();
}

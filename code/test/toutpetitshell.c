#include "syscall.h"

int
main ()
{
    SpaceId newProc;
    char buffer[60];
    int i;
    char* prompt = "\n>";
    
    while (1) {
        PutString(prompt);
        i = 0;
        GetString(buffer, 60);
PutString("buffer : ");
PutString(buffer);     
        newProc = ForkExec(buffer);
PutString("ForkExec");
        Join (newProc);
PutString("Join ForkExec");

    }
}

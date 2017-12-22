#include "syscall.h"

/*
– Commandes classiques: ls, cd, pwd, exit, ...
– Lancement d'executables UNIX: !<nom>
– Lancement d'executables NachOS
*/


int
main ()
{
    //SpaceId newProc;
    char buffer[60];
    int i;
    char* prompt = "> ";
    PutChar('\n');
    
    while (1) {
        PutString(prompt);
        i = 0;
        GetString(buffer, 60);
        if (buffer[0] != '\0') {   
            int newProc = ForkExec(buffer);
            UserThreadJoin (newProc);
            
        } else if (buffer[0] != '.') {  
PutString("Erreur : commandes classiques: ls, cd, pwd, exit, ...");
        }
    }
}




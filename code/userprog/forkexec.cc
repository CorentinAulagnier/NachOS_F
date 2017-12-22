#include "forkexec.h"

void StartProcess(int arg) {

    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();

    machine->Run();
}


int do_ForkExec (char *s)
{
    OpenFile *executable = fileSystem->Open (s);
    AddrSpace *space;



    if (executable == NULL) {
        printf ("Unable to open file %s\n", s);
        delete [] s;
        return -1;
    }
    // Creation d'un nouvel espace d'adressage
    space = new AddrSpace (executable);

    if(space == NULL){
        printf("Impossible de créer un nouveau processus\n");
        return -1;
    }

    /* nbProcessus = nbProcessus +1 */
    machine->ajouterProcessus();
    
    delete executable;

    // Creation du nouveau thread main du nouveau processus
    Thread * main = new Thread(s);
    main->space = space;

    main->Fork(StartProcess, 0);

    return 0;
}

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

    if(space == NULL || space->validSpace == false){
        printf("\nImpossible de créer un nouveau processus : Mémoire insuffisante\n");
        return -1;
    }
    
    delete executable;

    /* nbProcessus = nbProcessus +1 */
    machine->ajouterProcessus();

    // Creation du nouveau thread main du nouveau processus
    Thread * main = new Thread(s);

    main->numStackInAddrSpace = 0;
    main->space = space;
    main->space->nbThreads ++;

    main->Fork(StartProcess, 0);

    return 0;
}

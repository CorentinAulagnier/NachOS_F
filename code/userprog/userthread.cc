#include "userthread.h"

void StartUserThread(int f) {
    
    // Initialise les registres à 0, et calque les pages machine aux pages de l'adresse space
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();

    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, f);
    machine->WriteRegister(NextPCReg, f + 4);
    machine->WriteRegister(4,  currentThread->argUser);

    machine->WriteRegister(StackReg, machine->ReadRegister(PCReg) - (3 * PageSize));

    machine->Run();
}

int do_UserThreadCreate(int f, int arg) {

    /* Création du nouveau thread */
    Thread* newThread = new Thread("Thread créé");
    if (newThread == NULL) return 0;

    /* Sauvegarde de l'argument de f // TODO si bug : arg pas NULL*/
    newThread->argUser = arg;

    /* Initialisation et Placement dans la file d'attente des threads noyaux */
    newThread->Fork(StartUserThread, f);

    return 1;
}



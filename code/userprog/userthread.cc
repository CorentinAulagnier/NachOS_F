#include "userthread.h"

void StartUserThread(int f) {
    
    // Initialise les registres à 0, et calque les pages machine aux pages de l'adresse space
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();

    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, f);
    machine->WriteRegister(NextPCReg, f + 4);
    machine->WriteRegister(4,  currentThread->argUser);

    
    int decalage = currentThread->space->CalculOffsetStack(NbPagesPileThread,PageSize,
currentThread->numStackInAddrSpace);

    machine->WriteRegister(StackReg, decalage);

    machine->Run();
}

int do_UserThreadCreate(int f, int arg) {

    currentThread->space->semNbThread->P();

    /* Ajouté :
     * On ajoute un thread au processus + On enregistre le numéro du thread
     */
    int test = currentThread->space->structNbThreads->Find();
    if(test == -1){
        currentThread->space->semNbThread->V();
        return 0;
    } else {
        currentThread->numStackInAddrSpace = test ;
        currentThread->space->nbThreads ++;
    } 
    currentThread->space->semNbThread->V();

    /* Création du nouveau thread */
    Thread* newThread = new Thread("Thread créé");
    if (newThread == NULL) return 0;

    /* Sauvegarde de l'argument de f // TODO si bug : arg pas NULL*/
    newThread->argUser = arg;

    /* Initialisation et Placement dans la file d'attente des threads noyaux */
    newThread->Fork(StartUserThread, f);

    return 1;
}

int do_UserThreadExit() {

    currentThread->Finish();
    
    return 1;
}

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
    }
    currentThread->space->nbThreads ++;
    
    
    currentThread->space->semNbThread->V();

    /* Création du nouveau thread */
    Thread* newThread = new Thread("Thread créé");
    if (newThread == NULL) return 0;

    newThread->numStackInAddrSpace = test ;
    /* Sauvegarde de l'argument de f // TODO si bug : arg pas NULL*/
    newThread->argUser = arg;
    
    
    
    itemThread* it = newItemThread(newThread->tid);
    currentThread->space->listThread->Append(it);
    
    /*
    if (newThread->tid == 2){
        //currentThread->space->listThread->lock->P();
        currentThread->space->semNbThread->P();
    }
*/
    
    /* Initialisation et Placement dans la file d'attente des threads noyaux */
    newThread->Fork(StartUserThread, f);

    return newThread->tid;
}


void do_UserThreadExit() {

    currentThread->space->semNbThread->P();
    
    if(currentThread->space->nbThreads <= 0 ) {
        currentThread->space->semNbThread->V();
        printf("Exception: appel erroné à la fonction UserThreadExit\n");
    } else {
        currentThread->space->nbThreads --;
        /*
        if (currentThread->space->nbThreads == 0){
            terminaison->V();
        }
        */

        itemThread* it = currentThread->space->listThread->Find(currentThread->tid);
        it->semThread->V();
        

        currentThread->space->structNbThreads->Clear(currentThread->numStackInAddrSpace);
        currentThread->space->semNbThread->V();
        currentThread->Finish();
        //delete currentThread;
    }
}

void do_UserThreadJoin(int tid) {
    itemThread* it = currentThread->space->listThread->Find(tid);
    if (it != NULL)  {
        it->semThread->P();
    } else {
        printf("Exception: le thread spécifié lors de l'appel à UserThreadJoin n'existe pas.\n");
    }
    
}

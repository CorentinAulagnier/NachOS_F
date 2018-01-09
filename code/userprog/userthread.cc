#include "userthread.h"


void StartUserThread(int f) {   
    // Initialise les registres à 0, et calque les pages machine aux pages de l'adresse space
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();

    machine->WriteRegister(PrevPCReg, machine->ReadRegister(PCReg));
    machine->WriteRegister(PCReg, f);
    machine->WriteRegister(NextPCReg, f + 4);
    machine->WriteRegister(4,  currentThread->argUser);

    // Change la valeur de retour du thread : il doit s'arrêter une fois la fonction terminée !
    machine->WriteRegister(31,  currentThread->fonction_retour);

    int decalage = currentThread->space->CalculOffsetStack(NbPagesPileThread,PageSize,
currentThread->numStackInAddrSpace);

    machine->WriteRegister(StackReg, decalage);

    machine->Run();
}

int do_UserThreadCreate(int f, int arg, int fct_fin) {

    currentThread->space->semNbThread->P();

    /* On ajoute un thread au processus + On enregistre le numéro du thread*/
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
    newThread->estProcessus = false;

    newThread->fonction_retour = fct_fin;

    /* Sauvegarde de l'argument de f */
    newThread->argUser = arg;
      
    itemThread* it = newItemThread(newThread->tid);
    currentThread->space->listThread->Append(it);
    
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
        /* Si le thread courrant est un processus : on arrete le processus */ 
		if(currentThread->estProcessus == true){

        	currentThread->space->semNbThread->V();

			machine->supprimerProcessus();

            /* Si c'etait le dernier processus : on arrete tout */
            if(machine->getNbProcessus() <= 0) {
                interrupt->Halt();
			}

            /* Destruction du processus*/     
			currentThread->space->tokill = true;
			currentThread->Finish();    

		} else {
			itemThread* it = currentThread->space->listThread->Find(currentThread->tid);
			it->semThread->V();
			currentThread->space->structNbThreads->Clear(currentThread->numStackInAddrSpace);
        	currentThread->space->semNbThread->V();
        	currentThread->Finish();
		}
    }
}
    
void do_UserThreadJoin(int tid) {
    itemThread* it = currentThread->space->listThread->Find(tid);
    if (it != NULL)  {
        it->semThread->P();
        DEBUG('a', "Le thread %d s'est bien terminé.\n",it->tid);
        free(currentThread->space->listThread->Remove(it->tid));
        //pour le debug : etat de la liste
        //printList(currentThread->space->listThread);
    } else {
        printf("Exception: le thread spécifié lors de l'appel à UserThreadJoin n'existe pas.\n");
    }
    
}

void printList(SynchListThread* synchlist) {
    itemThread * elm = (itemThread *)synchlist->GetList()->GetFirst();
    int i = 0;
    if(elm==NULL) {
        printf("La liste est vide.\n");
    } else {
        printf("Etat de la liste :\n");
        while(elm!=NULL) {
            printf("élément %d - tid %d\n", i+1, elm->tid);
            elm = elm->next;
            i++;
        }
    }
}

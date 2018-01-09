// exception.cc 
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.  
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"
#include "forkexec.h"

//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
    int pc = machine->ReadRegister (PCReg);
    machine->WriteRegister (PrevPCReg, pc);
    pc = machine->ReadRegister (NextPCReg);
    machine->WriteRegister (PCReg, pc);
    pc += 4;
    machine->WriteRegister (NextPCReg, pc);
}

//----------------------------------------------------------------------
//vider
// remplit les size premiers caracteres de buff avec un '\0'
//----------------------------------------------------------------------
void vider(char* buff, int size){
    for(int i = 0; i<size; i++)
        buff[i] = '\0';
}


//----------------------------------------------------------------------
//copyStringFromMachine
// copie une chaîne du monde MIPS vers le monde Linux. 
// Au plus size caractères sont copiés. 
// Un ’\ 0’ est forcé à la fin de la copie en dernière position 
// pour garantir la sécurité du système.
//----------------------------------------------------------------------

char * copyStringFromMachine(int from, unsigned size) {
	
	int p;
	char * buf = new char[size];
    unsigned int i;
    for(i=0; i<size-1; i++){
	    bool b = machine->ReadMem(from+i,1, &p);

        if(b == FALSE){
            printf("false dans copy... !!!!!!!!!! \n");
            break;
        }
		if((char)p == '\0'){
			break;
		}

        buf[i] = (char)p;
    }
    buf[i] = '\0';
	return buf;
}

//----------------------------------------------------------------------
// writeStringToMachine
// copie une chaîne du monde Linux vers le monde MIPS. 
// Au plus size caractères sont copiés. 
// Un ’\ 0’ est forcé à la fin de la copie en dernière position 
// pour garantir la sécurité du système.
//----------------------------------------------------------------------

void writeStringToMachine(char * from, int to, unsigned size) {
    unsigned int i;
    //char *p = machine->mainMemory;
    for(i=0; i<size-1; i++){
        //p[to+i] = from[i];
        bool b = machine->WriteMem(to+i,1, from[i]);

        if(b == FALSE){
            printf("false dans Write... !!!!!!!!!!\n");
            break;
        }

		if(from[i]=='\0') {
			break;
		}
    }
}

//----------------------------------------------------------------------
//chercherTaille
// fonction comparable a strlen() sur le char * p a partir du char add
//----------------------------------------------------------------------

int chercherTaille(char * p, int add){
    int i = 0;
    while(p[add+i] != '\0'){
        i++;
    }
    return i;
}


//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions 
//      are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister (2);

    if (which == SyscallException) {
        switch (type) {
            case SC_Halt: {
                //terminaison->P();
                DEBUG('a', "Shutdown, initiated by user program.\n");
                interrupt->Halt();
                break;
            }
            case SC_PutChar: {
                //DEBUG('a', "SC_PutChar\n");
                char c = (char) machine->ReadRegister(4);
                synchconsole->SynchPutChar(c);
                break;
            }
            case SC_PutString: {
                //DEBUG('a', "SC_PutString\n");
                int add = machine->ReadRegister(4); // @ de la chaine
                
                char * s = copyStringFromMachine(add, MAX_STRING_SIZE);



                // Ecriture termine, on ecrit et on vide le buffer
                synchconsole->SynchPutString(s);


                break;
            } case SC_GetChar: {
                //DEBUG('a', "SC_GetChar\n");
                char c = synchconsole->SynchGetChar();
                machine->WriteRegister(2, (int)c);
                break;
            } case SC_GetString: {
                //DEBUG('a', "SC_GetString\n");
                int word = machine->ReadRegister(4);
                int taille = machine->ReadRegister(5);
                char s[taille];
                synchconsole->SynchGetString(s, taille);
                writeStringToMachine(s, word, taille);
                break;
            } case SC_GetInt: {
                //DEBUG('a', "SC_GetInt\n");
                int n = synchconsole->SynchGetInt();
                machine->WriteRegister(2, n);
                break;
            } case SC_PutInt: {
                //DEBUG('a', "SC_PutInt\n");
                int n = machine->ReadRegister(4);
                synchconsole->SynchPutInt(n);
                break;
            } case SC_UserThreadCreate: {
                //DEBUG('a', "\nSC_UserThreadCreate\n");
                int func = (int)machine->ReadRegister(4); // @ de la fonction
                int add = (int)machine->ReadRegister(5); // @ espace mem

                int fct_fin = (int)machine->ReadRegister(6); // @ espace mem

                int retour = do_UserThreadCreate(func, add, fct_fin);

                machine->WriteRegister(2, retour);
                break;
            } case SC_UserThreadExit: {
                //DEBUG('a', "\nSC_UserThreadExit\n");
                do_UserThreadExit();
                break;
            } case SC_UserThreadJoin: {
                //DEBUG('a', "\nSC_UserThreadJoin");
                int tid = (int)machine->ReadRegister(4); // @ de la fonction
                //printf(" tid %d\n",tid);
                do_UserThreadJoin(tid);
                break;
            } case SC_ForkExec: {
                printf("\nSC_ForkExec\n");

                int add = machine->ReadRegister(4); // @ de la chaine
                
                char *filename = copyStringFromMachine(add, MAX_STRING_SIZE);
            	int retour = do_ForkExec(filename);

				machine->WriteRegister(2,retour);
                break;
           
            } default: {
                DEBUG('a', "Unexpected user mode exception %d %d\n", which, type);
                ASSERT(FALSE);
            }
        }
        // LB: Do not forget to increment the pc before returning!
        UpdatePC ();
        // End of addition
    }
}





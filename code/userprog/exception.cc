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

void copyStringFromMachine(int from, char *to, unsigned size) {
    char *p = machine->mainMemory;
    unsigned int i;
    for(i=0; i<size; i++){
        to[i] = p[from+i];
    }
    to[i] = '\0';
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
                DEBUG('a', "Shutdown, initiated by user program.\n");
                interrupt->Halt();
                break;
            }
            case SC_PutChar: {
                //printf("SC_PutChar\n");
                char c = (char) machine->ReadRegister(4);
                synchconsole->SynchPutChar(c);
                break;
            }
            case SC_PutString: {
                //printf("SC_PutString\n");
                int add = (int) machine->ReadRegister(4); // @ de la chaine
                
                char* buffer = (char*)malloc(MAX_STRING_SIZE);
                int positionBuffer = 0;
                
                char * s = machine->mainMemory;
                int taille = chercherTaille(s, add);

                char *word = (char*) malloc(taille); 
                vider(buffer, taille);
                copyStringFromMachine(add, word, taille);

                unsigned int i = 0;
                while(word[i] != '\0') {
                    // Buffer plein, on l'ecrit et on le vide
                    if (positionBuffer == MAX_STRING_SIZE) {
                        synchconsole->SynchPutString(buffer);
                        vider(buffer, MAX_STRING_SIZE);
                        positionBuffer = 0;
                    }
                    // Ajout du s dans le buffer lettre par lettre
                    buffer[positionBuffer] = word[i];
                    positionBuffer++;
                    i++;
                }
                // Ecriture termine, on ecrit et on vide le buffer
                synchconsole->SynchPutString(buffer);
                vider(buffer, chercherTaille(buffer, 0));
                positionBuffer = 0;
                free(buffer);
                break;
            } case SC_GetChar: {
                printf("SC_GetChar\n");
                char c = synchconsole->SynchGetChar();
                machine->WriteRegister(2, c);
                break;
            } case SC_GetString: {
                printf("SC_GetString\n");
                char * c = (char *)malloc(MAX_BUFFER_SIZE);
                synchconsole->SynchGetString(c, MAX_BUFFER_SIZE);
                machine->WriteRegister(2, c);
                //                machine->WriteRegister(int, int);
                break;
            } case SC_GetInt: {
                printf("SC_GetInt\n");
                int n = 0;
                synchconsole->SynchGetInt(&n);
                printf("val n : %d\n",n);
                machine->WriteRegister(2, n);
                break;
            } case SC_PutInt: {
                printf("SC_PutInt\n");
                int n = machine->ReadRegister(4);
                synchconsole->SynchPutInt(n);
                break;
            }
            default: {
                printf("Unexpected user mode exception %d %d\n", which, type);
                ASSERT(FALSE);
            }
        }
        // LB: Do not forget to increment the pc before returning!
        UpdatePC ();
        // End of addition
    }
}






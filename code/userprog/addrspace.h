// addrspace.h 
//      Data structures to keep track of executing user programs 
//      (address spaces).
//
//      For now, we don't keep any information about address spaces.
//      The user level CPU state is saved and restored in the thread
//      executing the user program (see thread.h).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef ADDRSPACE_H
#define ADDRSPACE_H

#include "copyright.h"
#include "filesys.h"
#include "bitmap.h"
#include "synch.h"

#define UserStackSize		4 * 4 * 1024	// increase this as necessary!
#define MaxNbThread		5	// Nombre max de thread par processus
#define NbPagesPileThread		3	// Nombre de page pour la pile de chaque thread

class AddrSpace
{
  public:
    AddrSpace (OpenFile * executable);	// Create an address space,
    // initializing it with the program
    // stored in the file "executable"
    ~AddrSpace ();		// De-allocate an address space

    void InitRegisters ();	// Initialize user-level CPU registers,
    // before jumping to user code

    void SaveState ();		// Save/restore address space-specific
    void RestoreState ();	// info on a context switch 

    /* Ajouté :
     * Bitmap gérant le nombre de threads d'un processus
     */
    BitMap* structNbThreads;

    /* Ajouté :
     * Nombre de thread en cours d'exécution pour le processus
     */
    int nbThreads;

    /* Ajouté :
     * Semaphore gérant l'exclusivité de structNbThreads et nbThreads
     */
    Semaphore* semNbThread;

    /* Ajouté :
     * Calcule l'offset de la pile du thread de numero numstack
     */
    int CalculOffsetStack(int nbPagePile, int pagesize, int numstack);

  private:
      TranslationEntry * pageTable;	// Assume linear page table translation
    // for now!
    unsigned int numPages;	// Number of pages in the virtual 
    // address space
};

#endif // ADDRSPACE_H

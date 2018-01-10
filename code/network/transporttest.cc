// nettest.cc 
//	Test out message delivery between two "Nachos" machines,
//	using the Post Office to coordinate delivery.
//
//	Two caveats:
//	  1. Two copies of Nachos must be running, with machine ID's 0 and 1:
//		./nachos -m 0 -o 1 &
//		./nachos -m 1 -o 0 &
//
//	  2. You need an implementation of condition variables,
//	     which is *not* provided as part of the baseline threads 
//	     implementation.  The Post Office won't work without
//	     a correct implementation of condition variables.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "transport.h"

void
EnvoiTest(int farAddr)
{
    Transport* t = new Transport();
    char message[] = "Bonjour_je_suis_Corentin_!";
    
    if (t->send(farAddr, message, sizeof(message))) printf("Sending OK !\n");
    else printf("Sending ERROR !\n");
    
    interrupt->Halt();
}

void
ReceptionTest(int farAddr)
{
    Transport* t = new Transport();
    char message[MAX_BUFFER_SIZE];
    
    if (t->receive(farAddr, message)) printf("Receive OK ! \"%s\"\n",message);
    else printf("Receive ERROR !\n");
    
    interrupt->Halt();
}



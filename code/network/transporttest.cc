// nettest.cc 
//	Test out message delivery between two "Nachos" machines,
//	using the Post Office to coordinate delivery.
//
//	Two caveats:
//	  1. Two copies of Nachos must be running, with machine ID's 0 and 1:
//		#include "network.h"./nachos -m 0 -o 1 &
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

#include "system.h"
#include "network.h"
#include "interrupt.h"
#include "transport.h"



void
EnvoiTest(int farAddr, void* message)
{
    Transport* t = new Transport();
    
    printf("Envoie du message : \"%s\" a la machine 1\n",(char*)message);
    
    if (t->send(farAddr, message, sizeof(message))) printf("Sending OK !\n");
    else printf("Sending ERROR !\n");

    interrupt->Halt();   
}

void EnvoiTest2(int farAddr){
    EnvoiTest(farAddr, (char*)"Bonjour_je_suis_Corentin_!Bonjour_je_suis_Corentin_!");
}

void
ReceptionTest(int farAddr)
{
    Transport* t = new Transport();
    char message[MAX_BUFFER_SIZE];
    bzero(message, MAX_BUFFER_SIZE);
    
    printf("En attente de message en provenance de la machine 0\n");
    
    if (t->receive(farAddr, message)) {
        printf("Receive OK ! \n");
        printf("Message : \n\"%s\"\n",message);
    }
    else printf("Receive ERROR !\n");

    interrupt->Halt();
   
}



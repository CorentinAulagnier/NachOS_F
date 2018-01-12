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



void vider(){
    
    MailBox* box = postOffice->GetBox(1);
    SynchList* messages = box->GetMessages();
    char data[] = "MAIL ERROR";
    PacketHeader outPktHdr;	  
    MailHeader outMailHdr;

    while (!messages->IsEmpty()){
        postOffice->Receive(1, &outPktHdr, &outMailHdr, data);
    }
    
printf("\nBoite vide\n");
}

void
EnvoiTest(int farAddr, float reli)
{
    Transport* t = new Transport(reli);
    
    char message[5][MAX_STRING_SIZE];
    strcpy(message[0],"Hello");
    strcpy(message[1], "_CouCou_!");
    strcpy(message[2], "%*µ£µµ%£2345678*");
    strcpy(message[3], "Bonjour_nous_sommes_le_groupe_F!");
    strcpy(message[4], "Une sphère mouvante désigne un ensemble de cercles figurant le système solaire en tout ou partie. ");

    for (int i = 0; i<5; i++) {

        printf("\n**********************\n\n");
        printf("Envoie du message : \"%s\" a la machine %d\n",message[i],farAddr);
        
        if (t->send(farAddr, message[i], strlen(message[i]))) printf("Sending OK !\n");
        else printf("Sending ERROR !\n");
        
        vider();
    }

    Delay (5);

    interrupt->Halt();   
}


void
ReceptionTest(int farAddr, float reli)
{
    Transport* t = new Transport(reli);
    
    char message[MAX_BUFFER_SIZE];
 
    for (int i = 0; i<5; i++) {


        printf("\n**********************\n\n");
        printf("En attente de message en provenance de la machine %d\n",farAddr);
     
        bzero(message, MAX_BUFFER_SIZE);

        if (t->receive(farAddr, message)) {
            printf("Receive OK ! \n");
            printf("Message : \n\"%s\"\n",message);
        }
        else printf("Receive ERROR !\n");
        
        vider();
    }

    interrupt->Halt();
   
}


#include "copyright.h"

#include "system.h"
#include "network.h"
#include "interrupt.h"
#include "transport.h"



void vider(){
    
    MailBox* box = postOffice->GetBox(1);
    if (box == NULL) return;
    SynchList* messages = box->GetMessages();
    if (messages == NULL) return;

    char data[] = "MAIL ERROR";
    PacketHeader outPktHdr;	  
    MailHeader outMailHdr;
    while (!messages->IsEmpty()){
        postOffice->Receive(1, &outPktHdr, &outMailHdr, data);
    }
}

void
EnvoiTest(int farAddr, float reli)
{
    Transport* t = new Transport(reli);
    vider();
    
    char message[5][MAX_STRING_SIZE];

    strcpy(message[0],"Hello");
    strcpy(message[1], "_CouCou_!");
    strcpy(message[2], "%*µ£µµ%£2345678*");
    strcpy(message[3], "Bonjour nous sommes le groupe F!");
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
    vider();
    
    char message[MAX_BUFFER_SIZE];
        
    for (int i = 0; i<5; i++) {


        printf("\n**********************\n\n");
        printf("En attente de message en provenance de la machine %d\n",farAddr);
     
        bzero(message, MAX_BUFFER_SIZE);

        if (t->receive(farAddr, message)) {
            printf("Receive OK ! \n");
            printf("Message : \n\"%s\"\n",message);
        }
        else {
            printf("Receive ERROR !\n");
        }
        
        vider();
    }

    interrupt->Halt();
   
}


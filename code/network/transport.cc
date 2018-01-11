#include "transport.h"

#define MAXREEMISSIONS 10
#define TEMPO 1 // en secondes
#define ACK "ACK" 

/* ------- map --------- */
/*
bool isMapped(int to) {
    int i;
    for(i = 0; i<postOffice->numBoxes; i++) {
        if(map[i] == to) return true;
    }
    return false;
}

bool addMapping(int to) {
    int i;
    for(i = 0; i<postOffice->numBoxes; i++) {
        if(map[i] == -1) {
            map[i] = to;
            return true;
        } 
    }
    return false;
}

int findBox(int to) {
    int i;
    for(i = 0; i<postOffice->numBoxes; i++) {
        if(map[i] == to) return i;
    }
    return -1;
}


*/

/* -------- transport -------*/

Transport::Transport()
{
   // reception = new Semaphore("synch reception", 0);
}

Transport::~Transport()
{ 
   // delete reception;
}

/*****************************************************************/
/* SEND                                                          */
/*****************************************************************/

bool Transport::send(int to, void* content, int sizeContent){
    /*
    // S ajoute dans la map si il n y est pas encore
    if(!isMapped(to)) {
        //sort s'il échoue
        if(!addMapping(to)) return false;
    }
    */
    char buff[MaxMailSize];
    bool trySuccess = true; // false = Erreur lors de l envoie
    
    int nbPaquets = sizeContent/MaxMailSize;
    if (sizeContent%MaxMailSize > 0 ) nbPaquets++;
/*
printf("Nombre de paquets :%d\n",nbPaquets);
*/
    for (int i = 0; i<=nbPaquets; i++){
        printf(".");
        fflush(stdout);
/*
printf("Traitement du paquet num %d\n",i);
*/
        if (i == 0){ // paquet initiale 
            /* Envoie de la taille du message */
            char size[MAX_INT_SIZE];
            sprintf(size,"%d", sizeContent);
            trySuccess = trySend(to, i, (void *)size, sizeof(size));

        } else { // paquet donnees
            /* Envoie des paquets */
            bzero(buff, MaxMailSize+1);
            for (int j = 0; j<(int)MaxMailSize; j++)
                buff[j] = ((char*)((int)content + (i-1)*(int)MaxMailSize))[j];
/*
printf("Message = %s\n", buff);
*/
            trySuccess = trySend(to, i, buff, strlen(buff));

        }
/*
printf("Paquet num %d, %s\n", i, trySuccess?"EnvoieSucces":"Envoie ERREUR");
*/
        if (!trySuccess) return false; 
    }
    
    Delay (MAXREEMISSIONS);
    return true;
}

bool trySend(int to, int numPaquet, void* content, int sizeMail){
    
    PacketHeader outPktHdr = creerPacketHeader(to, postOffice->GetAdd());	  
    MailHeader outMailHdr = creerMailHeader(1, 0, numPaquet, sizeMail);

    return sendingLoop(outPktHdr, outMailHdr, content);
}


bool sendingLoop(PacketHeader outPktHdr, MailHeader outMailHdr, void* content) {
    // Permier essai
    postOffice->Send(outPktHdr, outMailHdr, (char *)content);    
    
    // Boucle réémission avec vérif toutes les 5 secondes
    for (int i = 1; i < MAXREEMISSIONS; i++) {
        if(ackReceive(outMailHdr.to, outMailHdr.numPaquet)) {
            return true;
        }
        Delay (TEMPO);  
        postOffice->Send(outPktHdr, outMailHdr, (char *)content);
    }
    return false;
}

bool ackReceive(int fromMachine, int numPaquet){
    /*
    int numBox = findBox(to);
    //Box introuvable
    if(numBox == -1) return false;
    */
    
    MailBox* box = postOffice->GetBox(1);
    SynchList* messages = box->GetMessages();

    PacketHeader pktHdr;
    MailHeader mailHdr;
    char data[] = "MAIL ERROR";
    
    while (!messages->IsEmpty()){
        postOffice->Receive(1, &pktHdr, &mailHdr, data);
        
        if (pktHdr.from == fromMachine && mailHdr.numPaquet == numPaquet &&
        !strcmp(data,ACK)) {
            return true;
        }
    }
    //Aucun mail ne correspond
    return false;

}
/*****************************************************************/
/* RECEIVE                                                       */
/*****************************************************************/

bool Transport::receive(int from, void* content){
    /*
    int numBox = findBox(to);
    
    // Box introuvable 
    if(numBox == -1) return false;
    */
    
    PacketHeader pktHdr;
    MailHeader mailHdr;
    
    char buffer[MAX_BUFFER_SIZE];
    bzero(buffer, MAX_BUFFER_SIZE);
    int posBuffer = 0;
    
    char mail[MaxMailSize];
    bool ackSuccess = true;
    
    int nbPaquets = 1;
    
    for (int i = 0; i<=nbPaquets; i++){
        printf(".");
        fflush(stdout);
/*
printf("Traitement du paquet num %d\n",i);
*/   
        if (i == 0) {
            /* Reception taille fichier */
            postOffice->Receive(1, &pktHdr, &mailHdr, buffer); 
            
            /* Envoie ack taille fichier */
            ackSuccess = tryAck(pktHdr.from, i, false);
            
            /* Calcul du nb de paquets du message */
            nbPaquets = atoi(buffer)/MaxMailSize;
            if (atoi(buffer)%MaxMailSize > 0 ) nbPaquets++;
/*
printf("Nombre de paquets : %d\n",nbPaquets);
*/   
        } else {
            bool paquetNotFind = true;
            while (paquetNotFind) {
                /* Reception du paquetnumPaquet */
                postOffice->Receive(1, &pktHdr, &mailHdr, (char*)mail);

                if (mailHdr.numPaquet == i) {
/*
printf("Message recu = \"%s\"\n", mail);
*/
                    strncpy((char*)((int)buffer + posBuffer), mail, mailHdr.length);
                    posBuffer +=mailHdr.length;
                    
                    /* Envoie ack du numPaquet */
                    if (i == nbPaquets) { // Dernier ACK
                        ackSuccess = tryAck(pktHdr.from, i, true);
                    } else {
                        ackSuccess = tryAck(pktHdr.from, i, false);
                    }
                    paquetNotFind = false;
                    
                }
            }
        }
/* 
printf("Paquet num %d, %s\n",i, ackSuccess?"AckSuccess":"Ack ERREUR");
*/
        if (!ackSuccess) return false; 

    }

    strcpy((char*)content, buffer);
/*
printf("Contenue global du message = \n\"%s\"\n",(char*)content);
*/
    //Delay (MAXREEMISSIONS); // Attente des dernieres emmissions d ACK
    return true;
}

bool tryAck(int to, int numPaquet, bool lastAck){
    
    PacketHeader outPktHdr = creerPacketHeader(to, postOffice->GetAdd());	  
    MailHeader outMailHdr = creerMailHeader(1, 0, numPaquet, sizeof(ACK));

    return sendingAckLoop(outPktHdr, outMailHdr, lastAck);
}

bool sendingAckLoop(PacketHeader outPktHdr, MailHeader outMailHdr, bool lastAck){

    postOffice->Send(outPktHdr, outMailHdr, ACK);

    for (int i = 1; i < MAXREEMISSIONS; i++) {
        if(nextReceive(outPktHdr.to, outMailHdr.numPaquet)) {
            return true;
        }
        Delay (TEMPO);
        postOffice->Send(outPktHdr, outMailHdr, ACK);
    }
    return lastAck;
}

bool nextReceive(int fromMachine, int numPaquet){
    /*
    int numBox = findBox(to);
    
    //Box introuvable
    if(numBox == -1) return false;
    */
    
    MailBox* box = postOffice->GetBox(1);
    SynchList* messages = box->GetMessages();

    PacketHeader pktHdr;
    MailHeader mailHdr;
    char data[MaxMailSize];
    
    while (!messages->IsEmpty()){
        postOffice->Receive(1, &pktHdr, &mailHdr, data);

        if (pktHdr.from == fromMachine && mailHdr.numPaquet == numPaquet+1) {
            // Remise du paquet dans la boite
            return true;
        }
    }
    //Aucun mail ne correspond
    return false;
}


/*****************************************************************/
/* TOOLS                                                         */
/*****************************************************************/

PacketHeader creerPacketHeader(int to, int from){
    PacketHeader outPktHdr;
    outPktHdr.to = to;
    outPktHdr.from = from;  
    return outPktHdr;
}

MailHeader creerMailHeader(int numBoxTo, int numBoxFrom, int numPaquet, int size){
    MailHeader outMailHdr;
    outMailHdr.to = numBoxTo;
    outMailHdr.from = numBoxFrom;
    outMailHdr.length = size;
    outMailHdr.numPaquet = numPaquet;
    return outMailHdr;
}

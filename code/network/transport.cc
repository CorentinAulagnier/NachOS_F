#include "transport.h"

static int MAXREEMISSIONS;
static int DELAY_END;
static float TEMPO;

/* -------- transport -------*/

Transport::Transport(float reemission)
{
    if (reemission == 1) {
        MAXREEMISSIONS = 10;
        TEMPO = 1;
        DELAY_END = 5;
    } else if (reemission >= 0.5) {
        MAXREEMISSIONS = 15;
        TEMPO = 1;
        DELAY_END = 10;
    } else if (reemission >= 0.3) {
        MAXREEMISSIONS = 25;
        TEMPO = 1;
        DELAY_END = 10;
    } else  {
        MAXREEMISSIONS = 70;
        TEMPO = 1;
        DELAY_END = 20;
    }
}

Transport::~Transport()
{
}

/*****************************************************************/
/* SEND                                                          */
/*****************************************************************/

bool Transport::send(int to, void* content, int sizeContent){
    char buff[MaxMailSize];
    bool trySuccess = true; // false = Erreur lors de l envoie
    
    int nbPaquets = sizeContent/MaxMailSize;
    if (sizeContent%MaxMailSize > 0 ) nbPaquets++;
    
    DEBUG('w',"sizeContent = %d, Nombre de paquets :%d\n",sizeContent, nbPaquets);

    for (int i = 0; i<=nbPaquets; i++){
        printf(".");
        fflush(stdout);

        DEBUG('w',"Traitement du paquet num %d\n",i);

        if (i == 0){ // paquet initiale 
            /* Envoie de la taille du message */
            char size[MAX_INT_SIZE];
            sprintf(size,"%d", sizeContent);
            trySuccess = trySend(to, i, (void *)size, strlen(size));
            
        } else { // paquet donnees
            /* Envoie des paquets */
            bzero(buff, MaxMailSize+1);
            for (int j = 0; j<(int)MaxMailSize; j++)
                buff[j] = ((char*)((int)content + (i-1)*(int)MaxMailSize))[j];

            DEBUG('w',"Message = %s\n", buff);

            trySuccess = trySend(to, i, buff, strlen(buff));

        }

        DEBUG('w',"Paquet num %d, %s\n", i, trySuccess?"EnvoieSucces":"Envoie ERREUR");

        if (!trySuccess) return false; 
    }

    Delay (DELAY_END);
    
    return true;
}

bool trySend(int to, int numPaquet, void* content, int sizeMail){
    
    PacketHeader outPktHdr = creerPacketHeader(to, postOffice->GetAdd());	  
    MailHeader outMailHdr = creerMailHeader(1, 0, numPaquet, sizeMail, 0);

    return sendingLoop(outPktHdr, outMailHdr, content);
}


bool sendingLoop(PacketHeader outPktHdr, MailHeader outMailHdr, void* content) {
    // Permier essai
    postOffice->Send(outPktHdr, outMailHdr, (char *)content);    
    
    // Boucle réémission avec vérif toutes les 5 secondes
    for (int i = 1; i < MAXREEMISSIONS; i++) {
        if(ackReceive(outPktHdr.to, outMailHdr.numPaquet)) {
            return true;
        }
        Delay (TEMPO);  
        postOffice->Send(outPktHdr, outMailHdr, (char *)content);
    }
    return false;
}

bool ackReceive(int fromMachine, int numPaquet){
    MailBox* box = postOffice->GetBox(1);
    SynchList* messages = box->GetMessages();

    PacketHeader pktHdr;
    MailHeader mailHdr;
    char data[] = "MAIL ERROR";

    while (!messages->IsEmpty()){
        postOffice->Receive(1, &pktHdr, &mailHdr, data);
        if (pktHdr.from == fromMachine && mailHdr.numPaquet == numPaquet && mailHdr.ack == 1){
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
    PacketHeader pktHdr;
    MailHeader mailHdr;
    
    char buffer[MAX_BUFFER_SIZE];
    bzero(buffer, MAX_BUFFER_SIZE);
    int posBuffer = 0;
    
    char mail[MaxMailSize];
    bzero(mail, MAX_BUFFER_SIZE);
    
    bool ackSuccess = true;
    
    int nbPaquets = 1;
    
    for (int i = 0; i<=nbPaquets; i++){
        printf(".");
        fflush(stdout);

        DEBUG('w',"Traitement du paquet num %d\n",i);
  
        if (i == 0) {
            /* Reception taille fichier */
            postOffice->Receive(1, &pktHdr, &mailHdr, buffer);
            while (mailHdr.ack == 1) {
                postOffice->Receive(1, &pktHdr, &mailHdr, buffer);
            }
            
            /* Envoie ack taille fichier */
            ackSuccess = tryAck(pktHdr.from, i, false);

            /* Calcul du nb de paquets du message */
            nbPaquets = atoi(buffer)/MaxMailSize;
            if (atoi(buffer)%MaxMailSize > 0 ) nbPaquets++;

            DEBUG('w',"Nombre de paquets : %d\n",nbPaquets);
  
        } else {
            bool paquetNotFind = true;
            while (paquetNotFind) {
                /* Reception du paquetnumPaquet */
                postOffice->Receive(1, &pktHdr, &mailHdr, (char*)mail);

                
                if (mailHdr.numPaquet == i) {
                    strncpy((char*)((int)buffer + posBuffer), mail, mailHdr.length);

                    posBuffer +=mailHdr.length;
                    buffer[posBuffer] = '\0';
                    
                    DEBUG('w',"buffer = %s\n",buffer);

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

        DEBUG('w',"Paquet num %d, %s\n",i, ackSuccess?"AckSuccess":"Ack ERREUR");

        if (!ackSuccess) return false; 

    }

    strcpy((char*)content, buffer);

    DEBUG('w',"Contenu global du message = \n\"%s\"\n",(char*)content);

    //Delay (MAXREEMISSIONS); // Attente des dernieres emmissions d ACK
    return true;
}

bool tryAck(int to, int numPaquet, bool lastAck){
    
    PacketHeader outPktHdr = creerPacketHeader(to, postOffice->GetAdd());	  
    MailHeader outMailHdr = creerMailHeader(1, 0, numPaquet, 0, 1);

    return sendingAckLoop(outPktHdr, outMailHdr, lastAck);
}

bool sendingAckLoop(PacketHeader outPktHdr, MailHeader outMailHdr, bool lastAck){

    postOffice->Send(outPktHdr, outMailHdr, "");

    int reemission = MAXREEMISSIONS;
    if (lastAck) reemission = (int)reemission/2;
    
    for (int i = 1; i < reemission; i++) {

        if(nextReceive(outPktHdr.to, outMailHdr.numPaquet)) {

            return true;
        }
        Delay (TEMPO);
        postOffice->Send(outPktHdr, outMailHdr, "");
    }
    return lastAck;
}

bool nextReceive(int fromMachine, int numPaquet){    
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

MailHeader creerMailHeader(int numBoxTo, int numBoxFrom, int numPaquet, int size, int ack){
    MailHeader outMailHdr;
    outMailHdr.to = numBoxTo;
    outMailHdr.from = numBoxFrom;
    outMailHdr.length = size;
    outMailHdr.numPaquet = numPaquet;
    outMailHdr.ack = ack;
    return outMailHdr;
}

void Transport::viderBoites(){
    
    MailBox* box = postOffice->GetBox(1);
    SynchList* messages = box->GetMessages();
    char data[] = "MAIL ERROR";
    PacketHeader outPktHdr;	  
    MailHeader outMailHdr;

    while (!messages->IsEmpty()){
        postOffice->Receive(1, &outPktHdr, &outMailHdr, data);
    }
    
    box = postOffice->GetBox(0);
    messages = box->GetMessages();
    
    while (!messages->IsEmpty()){
        postOffice->Receive(0, &outPktHdr, &outMailHdr, data);
    }
    
}

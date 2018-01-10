#include "transport.h"

#define MAXREEMISSIONS 5
#define TEMPO 1 // en secondes


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

bool Transport::send(int to, void* content, int sizeContent){
/*
    //s'ajoute dans la map si il n'y est pas encore
    if(!isMapped(to)) {
        //sort s'il échoue
        if(!addMapping(to)) return false;
    }
*/

    char buff[MaxMailSize];
    
    int nbPaquets = sizeContent%MaxMailSize;
    bool trySuccess = true;
    
    for (int i = 0; i<=nbPaquets; i++){
        if (i == 0){ // paquet initiale 
            trySuccess = tryInitSend(to, sizeContent);
        } else { // paquet donnees
            unsigned int j;
            for ( j = i*MaxMailSize; j<(i+1)*MaxMailSize;j++) {
                buff[j%MaxMailSize] = ((char *)content)[j];
            }
            trySuccess = trySend(to, i, (void *)buff);

        }   
        
        if (!trySuccess) return trySuccess; 
    }

    return true;
}

bool Transport::receive(int from, void* content){
    //int numBox = findBox(to);
    
    // Box introuvable
  //  if(numBox == -1) return false;
  
    MailBox* box = postOffice->GetBox(from);
    SynchList* messages = box->GetMessages();
    Mail *mail = (Mail *) messages->Remove();
    
    PacketHeader pktHdr = mail->pktHdr;
    MailHeader mailHdr = mail->mailHdr;
    char size[10];
    bcopy(mail->data, size, mail->mailHdr.length);
    
    // reception du 1er message
    int nbPaquets = atoi(size)%MaxMailSize;
    
    for (int i = 0; i<=nbPaquets; i++){

        //Mail *mail = (Mail *) postOffice->boxes[numBox]->messages->Remove();
        mail = (Mail *) messages->Remove();
        pktHdr = mail->pktHdr;
        mailHdr = mail->mailHdr;
        bcopy(mail->data, content, mailHdr.length);
        
        content = (void *)((int)content + mailHdr.length);
    
    
        PacketHeader outPktHdr = creerPacketHeader(pktHdr.from);	  
        MailHeader outMailHdr = creerMailHeader(mailHdr.from, mailHdr.numPaquet);
        postOffice->Send(outPktHdr, outMailHdr, "ACK");
    }
    return true;
}


bool trySend(int to, int numPaquet, void* content){
    
    PacketHeader outPktHdr = creerPacketHeader(to);	  
    MailHeader outMailHdr = creerMailHeader(to, numPaquet);

    return sendingLoop(outPktHdr, outMailHdr, content, numPaquet);
}

bool tryInitSend(int to, int sizeContent){

    PacketHeader outPktHdr = creerPacketHeader(to);	  
    MailHeader outMailHdr = creerMailHeader(to, 0);
    char content[sizeof(int)];
    content[0] = sizeContent;

    return sendingLoop(outPktHdr, outMailHdr, content, 0);

}

bool sendingLoop(PacketHeader outPktHdr, MailHeader outMailHdr, void* content, int numPaquet) {
    //permier essai
    postOffice->Send(outPktHdr, outMailHdr, (char *)content);
    //boucle réémission avec vérif toutes les 5 secondes (TODO refaire en utilisant une notification)
    for (int i = 1; i < MAXREEMISSIONS; i++) {
        if(ackReceive(outMailHdr.to, numPaquet)) return true;
        
        /* PROBLEME */
       // sleep(TEMPO);

        //if(packetRecieved(outMailHdr.to)) return true; 
        postOffice->Send(outPktHdr, outMailHdr, (char *)content);
    }
    return false;
}

bool ackReceive(int to, int numPaquet){
    //int numBox = findBox(to);
    
    // Box introuvable
   // if(numBox == -1) return false;

   // SynchList messages = postOffice->boxes[numBox]->messages;
   
    MailBox* box = postOffice->GetBox(to);
    SynchList* messages = box->GetMessages();
    
    Mail *mail;
    PacketHeader pktHdr;
    MailHeader mailHdr;
    
    // Pas de message en attente
    if(messages->IsEmpty()) return false;
    
    SynchList* saveMessages = new SynchList ();
    
    // TQ la boite n'est pas vide
    while (!messages->IsEmpty()){
        mail = (Mail *) messages->Remove();
        pktHdr = mail->pktHdr;
        mailHdr = mail->mailHdr;
        char data[MaxMailSize];
        bcopy(mail->data, data, mail->mailHdr.length);
        
        if (mailHdr.from == to && mailHdr.numPaquet != numPaquet &&
        strcmp(data,"ACK")) {
            //On remet les mails dans la box
            while(!saveMessages->IsEmpty()) {
                messages->Append(saveMessages->Remove());
            }
            return true;
        }   
        saveMessages->Append(mail);
    }
    
    //Aucun mail ne correspond
    messages = saveMessages;
    
    return false;
}

PacketHeader creerPacketHeader(int to){
    PacketHeader outPktHdr;
    outPktHdr.to = to;
    return outPktHdr;
}

MailHeader creerMailHeader(int to, int numPaquet){
    MailHeader outMailHdr;
    outMailHdr.to = to;
    outMailHdr.from = postOffice->GetAdd();
    outMailHdr.length = MaxMailSize;
    outMailHdr.numPaquet = numPaquet;
    return outMailHdr;
}

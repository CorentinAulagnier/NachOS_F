#include "system.h"
#include "addrspace.h"
#include "post.h"

#define ACK "ACK" 

class Transport {
    public :
        Transport(float reemission);
        ~Transport();

        /* Envoie un message content de taille sizeContent a la machine to
         * retourne 0 si cela ne marche pas, retourne 1 en cas de succes
         */
        bool send(int to, void* content, int sizeContent);
        
        /* Attend la reception d un message de la part de from
         * et l'enregistre dans content
         * retourne 0 si cela ne marche pas, retourne 1 en cas de succes
         */
        bool receive(int from, void* content);
        
        
    private :
      //  Semaphore reception;
};


/*****************************************************************/
/* SEND                                                          */
/*****************************************************************/

/* Cree les PacketHeader, et MailHeader
 * et lance sendingLoop
 * retourne 0 si cela ne marche pas, retourne 1 en cas de succes
 */
bool trySend(int to, int numPaquet, void* content, int sizeMail);

/* Envoie en boucle tant que !ackReceive()
 * retourne 0 si cela ne marche pas, retourne 1 en cas de succes
 */
bool sendingLoop(PacketHeader outPktHdr, MailHeader outMailHdr, void* content);

/* Cherche l ACK correspondant dans la boite mail
 * retourne 1 si l ACK est trouvé, retourne 0 sinon
 */
bool ackReceive(int fromMachine, int numPaquet);


/*****************************************************************/
/* RECEIVE                                                       */
/*****************************************************************/

/* Cree les PacketHeader, et MailHeader
 * et lance sendingAckLoop
 * retourne 0 si cela ne marche pas, retourne 1 en cas de succes
 */
bool tryAck(int to, int numPaquet, bool lastAck);

/* Envoie en boucle tant que !nextReceive()
 * retourne 0 si cela ne marche pas, retourne 1 en cas de succes
 */
bool sendingAckLoop(PacketHeader outPktHdr, MailHeader outMailHdr, bool lastAck);

/* Cherche si le prochain message est dans la boite mail
 * retourne 1 si l ACK est trouvé, retourne 0 sinon
 */
bool nextReceive(int fromMachine, int numPaquet);


/*****************************************************************/
/* TOOLS                                                         */
/*****************************************************************/

/* Cree les PacketHeader et MailHeader correspondant au parametres
 */
PacketHeader creerPacketHeader(int to, int from);
MailHeader creerMailHeader(int numBoxTo, int numBoxFrom, int numPaquet, int size, int ack);


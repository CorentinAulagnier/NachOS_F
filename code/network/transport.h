#include "system.h"
#include "post.h"

class Transport {
    public :
        Transport();
        ~Transport();

        // return 0 fail, 1 success
        bool send(int to, void* content, int sizeContent);
        bool receive(int from, void* content);
        
        /* ----- map ----- */
        /*bool isMapped(int to);
        bool addMapping(int to);
        int findBox(int to);
        int map[postOffice->numBoxes] = {-1}; //associe chaque autre machine connectée à une box
*/
    private :
      //  Semaphore reception;
};

bool trySend(int to, int numPaquet, void* content, int sizeMail);
bool sendingLoop(PacketHeader outPktHdr, MailHeader outMailHdr, void* content);
bool ackReceive(int fromMachine, int numPaquet);

bool tryAck(int to, int numPaquet, bool lastAck);
bool sendingAckLoop(PacketHeader outPktHdr, MailHeader outMailHdr, bool lastAck);
bool nextReceive(int fromMachine, int numPaquet);

PacketHeader creerPacketHeader(int to, int from);
MailHeader creerMailHeader(int numBoxTo, int numBoxFrom, int numPaquet, int size);

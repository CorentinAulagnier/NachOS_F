#include "system.h"
#include "post.h"
#include "synch.h"

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


bool trySend(int to, int numPaquet, void* content);
bool tryInitSend(int to, int sizeContent);
bool sendingLoop(PacketHeader outPktHdr, MailHeader outMailHdr, void* content, int numPaquet);
bool ackReceive(int to, int numPaquet);
PacketHeader creerPacketHeader(int to);
MailHeader creerMailHeader(int to, int numPaquet);

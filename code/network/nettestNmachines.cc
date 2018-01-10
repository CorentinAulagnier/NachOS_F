// nettestNmachines.cc 
//	Test out message delivery between N "Nachos" machines,
//	using the Post Office to coordinate delivery.

#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"

// Test out message delivery, by doing the following:
//	1. send a message to the machine with ID "farAddr", at mail box #0
//	2. wait for the other machine's message to arrive (in our mailbox #0)
//	3. send an acknowledgment for the other machine's message
//	4. wait for an acknowledgement from the other machine to our 
//	    original message

void
MailTestNmachines(int mailAddr, int farAddr)
{
    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    const char *data = "Hello there!";
    const char *ack = "Got it!";
    char buffer[MaxMailSize];

    if (mailAddr == 0) {
        // construct packet, mail header for original message
        outPktHdr.to = farAddr;		
        outMailHdr.to = farAddr;
        outMailHdr.from = mailAddr;
        outMailHdr.length = strlen(data) + 1;

        // Send message
        postOffice->Send(outPktHdr, outMailHdr, data); 
    
        postOffice->Receive(mailAddr, &inPktHdr, &inMailHdr, buffer);
        printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
        fflush(stdout);
        
    } else {
        // Wait for the first message from the other machine
        postOffice->Receive(mailAddr, &inPktHdr, &inMailHdr, buffer);
        printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
        fflush(stdout);

        // construct packet, mail header for original message
        outPktHdr.to = farAddr;		
        outMailHdr.to = farAddr;
        outMailHdr.from = mailAddr;
        outMailHdr.length = strlen(data) + 1;

        // Send message
        postOffice->Send(outPktHdr, outMailHdr, data); 
    }

    // Then we're done!
    interrupt->Halt();
}

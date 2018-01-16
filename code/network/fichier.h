#include "system.h"
#include "transport.h"

#define FILENAME        "TestFile"
#define MAXNAMESIZE     20
#define MAXCONTENTSIZE  10
#define MAXREQUESTSIZE  32
#define MAXFILESIZE     5000
#define CONTENT         "1234567890"
#define CONTENTSIZE 	strlen(CONTENT)

class FileRequest {
    public:
        FileRequest(int type, char *fileName, int contentSize);
        FileRequest(int type, char *fileName);
        ~FileRequest();
        
        int type;// SEND = 0, RECEIVE = 1
        char fileName[MAXNAMESIZE];
        int contentSize;
};

void testRequest();
FileRequest *readRequest(char *buffRequest);
void createRequest(FileRequest *request, char *buffRequest);
void printRequest(FileRequest *request);

bool readFile(char *fileName, char *buffer, int contentSize);
bool createFile(char *fileName, char *content, int contentSize);

void Client(int to, float rely);
void Serveur(int to, float rely);
void ServeurLoop(int to, float rely);
void ClientLoop(int to, float rely);

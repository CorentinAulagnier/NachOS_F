#include "fichier.h"
float rely = 1;

FileRequest::FileRequest(int t, char *fname, int size) {
    type = t;
    strncpy(fileName, fname, MAXNAMESIZE);
    contentSize = size;
}

FileRequest::FileRequest(int t, char *fname) {
    type = t;
    strncpy(fileName, fname, MAXNAMESIZE);
}

FileRequest::~FileRequest() {
}



void testRequest() {
    FileRequest *request = new FileRequest(0, (char *)FILENAME, CONTENTSIZE);
    char *buffRequest = (char *)malloc(MAXREQUESTSIZE);
    createRequest(request, buffRequest);
    readRequest(buffRequest);
    readRequest((char*)"1bonjour.png$125");
    free(buffRequest);
}

//traduit un char* utilisable par la couche transport en un FileRequest
FileRequest *readRequest(char *buffRequest) {

    char fileName[MAXNAMESIZE];
    char buffSize[MAXCONTENTSIZE];
    int type = (buffRequest[0]=='0')?0:1;// SEND = 0, RECEIVE = 1
    int i = 1, j = 0, k = 0, contentSize;
    bool isName = true;
    
    while(buffRequest[i] != '\0') {
        //nom du fichier
        if(isName) {
            if(buffRequest[i]=='$') { // $ est la separateur
                isName = false;
                fileName[j] = '\0';
            } else if(j<MAXNAMESIZE) {
                fileName[j] = buffRequest[i];
                j++;
            }
        //taille du fichier
        } else {
            buffSize[k] = buffRequest[i];
            k++;
        }
        i++; 
    }
    buffSize[k]='\0';
    contentSize = atoi(buffSize);
    FileRequest *request = new FileRequest(type, fileName, contentSize);
    
    //verifs
    
    printf("---------- VERIFS readRequest() ----------\n");
    printf("Request: |%s|\n", buffRequest);
    printRequest(request);
    printf("\n");

    return request;
}

//traduit un FileRequest en char* utilisable par la couche transport
void createRequest(FileRequest *request, char *buffRequest) {
    
    //type
    if(request->type==0) {
        buffRequest[0] = '0';
    } else if(request->type==1) {
        buffRequest[0] = '1';
    }
    
    //nom
    int i = 1;
    int length = strlen(request->fileName);
    while(i-1<length) {
        buffRequest[i] = request->fileName[i-1];
        i++;
    }
    
    //separateur
    buffRequest[i] = '$';
    i++;
    
    //taille
    char buffSize[MAXCONTENTSIZE];
    snprintf(buffSize, MAXCONTENTSIZE, "%d", request->contentSize);
    int j = 0;
    while(buffSize[j] != '\0') {
        buffRequest[i] = buffSize[j];
        i++;
        j++;
    }
    
    //verifs
    
    printf("---------- VERIFS createRequest() ----------\n");
    printRequest(request);
    printf("Request: |%s|\n", buffRequest);
    printf("\n");
    
}

void printRequest(FileRequest *request) {
    printf("FileRequest\n");
    char typeInString[10];
    strcpy(typeInString,"NULL");
    if(request->type==0)
        strcpy(typeInString,"SEND");
    else if(request->type==1)
        strcpy(typeInString,"RECEIVE");
        
    printf("\ttype: |%s|\n", typeInString);
    printf("\tfileName: |%s|\n",request->fileName);
    printf("\tcontentSize: |%d|\n",request->contentSize);
}

//crée un fichier et le sauve sur le fileSystem
bool createFile(char *fileName, char *content, int contentSize)
{
    OpenFile *openFile;    
    int numBytes;

    if (!fileSystem->Create(fileName, 0)) {
      printf("Impossible de creer le fichier %s\n", fileName);
      return false;
    }
    
    openFile = fileSystem->Open(fileName);
    if (openFile == NULL) {
	    printf("Impossible d'ouvrir le fichier %s\n", fileName);
	    return false;
    }

    numBytes = openFile->Write(content, contentSize);
    if (numBytes < contentSize) {
        printf("Impossible d'ecrire le fichier %s\n", fileName);
        delete openFile;
        return false;
    }
    
    //TODO besoin de "sauver" openFile (ou son fileHdr) ? Apparement non
    delete openFile;
    return true;
}

//lit un fichier dans fileSystem et retourne son contennu dans buffer
bool readFile(char *fileName, char *buffer, int contentSize)
{
    OpenFile *openFile;
    int numBytes;

    if ((openFile = fileSystem->Open(fileName)) == NULL) {
	    printf("Impossible d'ouvrir le fichier %s\n", fileName);
	    return false;
    }
    
    numBytes = openFile->Read(buffer, contentSize);
    if (numBytes < contentSize) {
        printf("Impossible de lire le fichier %s\n", fileName);
        delete openFile;
        return false;
    }
    
    delete openFile;	// close file
    return true;
}

void Client(int id) { //ID: 1+
    int to = 0;
    char *buffContent = (char *)malloc(MAXFILESIZE);
    char *buffRequest = (char *)malloc(MAXREQUESTSIZE);
    FileRequest *request;
    Transport *transport = new Transport(rely);
    
    //crée un fichier
    printf("Client: création du fichier %s\n", FILENAME);
    createFile((char *)FILENAME, (char *)CONTENT, CONTENTSIZE);
    
    //l'envoie au serveur
    printf("Client: envoie du fichier %s au serveur\n", FILENAME);
    
    request = new FileRequest(0, (char *)FILENAME, CONTENTSIZE);
    createRequest(request, buffRequest);
    transport->send(to, buffRequest, MAXREQUESTSIZE);
    
    readFile((char *)FILENAME, buffContent, CONTENTSIZE);
    transport->send(to, buffContent, CONTENTSIZE);
    
    //clean
    bzero(buffContent, MAXFILESIZE);
    bzero(buffRequest, MAXREQUESTSIZE);
    
    //le redemande au serveur
    printf("Client: demande du fichier %s au serveur\n", FILENAME);
    
    request = new FileRequest(1, (char *)FILENAME, CONTENTSIZE);
    createRequest(request, buffRequest);
    transport->send(to, buffRequest, MAXREQUESTSIZE);
    
    //TODO attend accord du serveur (ACK?)
    transport->receive(to, buffContent);
    createFile(request->fileName, buffContent, request->contentSize);
    
    //TODO compare les deux
    
    free(buffContent);
    free(buffRequest);
}

void Serveur(int id) { //ID: 0
    int to = 1;
    char *buffRequest = (char *)malloc(MAXREQUESTSIZE);
    char *buffContent = (char *)malloc(MAXFILESIZE);
    FileRequest *request;
    Transport *transport = new Transport(rely);
    
    while(1) {
        transport->receive(to, buffRequest);
        request = readRequest(buffRequest);
        switch(request->type) {//TODO
            case 0: { //le CLIENT envoie
                //repond ACK au client ?
                transport->receive(to, buffContent);
                createFile(request->fileName, buffContent, request->contentSize);
            }
            case 1: {//le CLILENT reçoit
                    readFile(request->fileName, buffContent, request->contentSize);
                    transport->send(to, buffContent, request->contentSize);                  
            }
            default : printf("Request error.\n");
        }
    }
}

#include "fichier.h"
FileRequest::FileRequest(int t, char *fname, int size) {
    type = t;
    strncpy(fileName, fname, MAXNAMESIZE);
    contentSize = size;
}

FileRequest::FileRequest(int t, char *fname) {
    type = t;
    strncpy(fileName, fname, MAXNAMESIZE);
    contentSize = 0;
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
    /*
    printf("---------- VERIFS readRequest() ----------\n");
    printf("Request: |%s|\n", buffRequest);
    printRequest(request);
    printf("\n");
    */
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
    /*
    printf("---------- VERIFS createRequest() ----------\n");
    printRequest(request);
    printf("Request: |%s|\n", buffRequest);
    printf("\n");
    */
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
    if(request->contentSize!=0) printf("\tcontentSize: |%d|\n",request->contentSize);
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

void Client(int to, float rely) { //ID: 1+
    char buffRequest[MAXREQUESTSIZE];
    char buffContent[MAXFILESIZE];
        //clean
    bzero(buffContent, MAXFILESIZE);
    bzero(buffRequest, MAXREQUESTSIZE);
    FileRequest *request;
    Transport *transport = new Transport(rely);
    
    char contentClient[] = "je_suis_le_client"; //17
    
    //crée un fichier
    //printf("Client: création du fichier %s\n", FILENAME);
    //createFile((char *)FILENAME, (char *)CONTENT, CONTENTSIZE);
    
    //l'envoie au serveur
    printf("Client: envoie du fichier %s au serveur\n", FILENAME);
    
    request = new FileRequest(0, (char *)FILENAME, strlen(contentClient));
    createRequest(request, buffRequest);

    printf("buffRequest: %s, size = %d\n",buffRequest,strlen(buffRequest));

    if (transport->send(to, buffRequest, strlen(buffRequest)))
    
        printf("send OK !\n");
        else printf("send ERROR !\n");
    printRequest(request); 
    
    //readFile((char *)FILENAME, buffContent, CONTENTSIZE);
    transport->send(to, contentClient, strlen(contentClient));
    
    //clean
    bzero(buffContent, MAXFILESIZE);
    bzero(buffRequest, MAXREQUESTSIZE);
    
    //le redemande au serveur
    printf("Client: demande du fichier %s au serveur\n", FILENAME);
    
    request = new FileRequest(1, (char *)FILENAME, 17);
    createRequest(request, buffRequest);
    transport->send(to, buffRequest, strlen(buffRequest));
    printRequest(request);
    
    //TODO attend accord du serveur (ACK?)
    transport->receive(to, buffContent);
    printf("content reçu : |%s|\n",buffContent);
    //createFile(request->fileName, buffContent, request->contentSize);
    
    //TODO compare les deux
}

void ClientLoop(int to, float rely) { //ID: 1+
    printf("BIENVENUE DANS LE CLIENT NACHOS\n");
    char buffRequest[MAXREQUESTSIZE];
    char buffContent[MAXFILESIZE];
    //clean
    bzero(buffContent, MAXFILESIZE);
    bzero(buffRequest, MAXREQUESTSIZE);
    FileRequest *request;
    Transport *transport = new Transport(rely);
    
    char contentClient[] = "je_suis_le_client"; //17
    
    while (1) {
        printf("\n\nQuelle requête voulez vous lancer ?\n");
        printf("\t 1 - Envoyer un fichier au serveur\n");
        printf("\t 2 - Demander un fichier au serveur\n");
        printf("\t 0 - Exit\n");
        char c = getchar();
        if(c=='\n') {
            c = getchar();
        }
        switch (c) {
            case '0':
                printf("\n***** Fin des tests ******\n\n");
                exit(0);
                break;
            case '1':
                //l'envoie au serveur
                printf("Client: envoie du fichier %s au serveur\n", FILENAME);
                
                request = new FileRequest(0, (char *)FILENAME, strlen(contentClient));
                createRequest(request, buffRequest);

                //printf("buffRequest: %s, size: %d\n",buffRequest,strlen(buffRequest));
                printRequest(request); 
                transport->send(to, buffRequest, strlen(buffRequest));

                
                //readFile((char *)FILENAME, buffContent, CONTENTSIZE);
                //transport->send(to, buffContent, strlen(buffContent));
                printf("Contenu envoyé : |%s|\n",contentClient);
                if(transport->send(to, contentClient, strlen(contentClient))) {
                    printf("Le fichier %s a bien été envoyé.\n",request->fileName);
                }
            break;
            
            case '2':
                //demande un fichier au serveur
                printf("Client: demande du fichier %s au serveur\n", FILENAME);
                
                request = new FileRequest(1, (char *)FILENAME);
                createRequest(request, buffRequest);
                printRequest(request);
                transport->send(to, buffRequest, strlen(buffRequest));

                transport->viderBoites();

                if(transport->receive(to, buffContent)) {
                    printf("Le fichier %s a bien été reçu.\n",request->fileName);
                }
                printf("Contenu reçu : |%s|\n",buffContent);
                //createFile(request->fileName, buffContent, request->contentSize);
                break;
            default :
                printf("Requête non reconnue\n\n");
        }
        
        //clean en fin d'iteration
        bzero(buffContent, MAXFILESIZE);
        bzero(buffRequest, MAXREQUESTSIZE);
        transport->viderBoites();
    }
    
    //creation un fichier
    //printf("Client: création du fichier %s\n", FILENAME);
    //createFile((char *)FILENAME, (char *)CONTENT, CONTENTSIZE);

}

void Serveur(int to, float rely) { //ID: 0
    char buffRequest[MAXREQUESTSIZE];
    char buffContent[MAXFILESIZE];
    FileRequest *request;
    Transport *transport = new Transport(rely);
    
    char contentServeur[] = "je_suis_le_serveur"; //18

    printf("\n --- WAIT REQUEST ---\n\n");  
    if (transport->receive(to, (void*)buffRequest)) 
        printf("buffRequest: %s\n",buffRequest);
    else
        printf("ERROR !!\n");
    
    
    
    request = readRequest(buffRequest);
    printRequest(request);

    printf("receive\n");
    transport->receive(to, buffContent);
    printf("content reçu : |%s|\n",buffContent);
    
    printf("\n --- WAIT REQUEST ---\n\n");
    transport->receive(to, buffRequest);
    request = readRequest(buffRequest);
    printRequest(request);
    
    printf("content envoyé : |%s|\n",contentServeur);
    transport->send(to, contentServeur, request->contentSize);                  

    
}

void ServeurLoop(int to, float rely) { //ID: 0
    char *buffRequest = (char *)malloc(MAXREQUESTSIZE);
    char *buffContent = (char *)malloc(MAXFILESIZE);
    FileRequest *request;
    Transport *transport = new Transport(rely);
    
    char contentServeur[] = "je_suis_le_serveur"; //18
    
    while(1) {
        printf("\n --- WAIT REQUEST ---\n\n");
        transport->receive(to, buffRequest);
        request = readRequest(buffRequest);
        printRequest(request);
        transport->viderBoites();
        switch(request->type) {//TODO
            case 0:  //le CLIENT envoie
                //repond ACK au client ?
                if(transport->receive(to, buffContent)) {
                    printf("Le fichier %s a bien été reçu.\n",request->fileName);
                }
                printf("Contenu reçu: |%s|\n",buffContent);
                //createFile(request->fileName, buffContent, request->contentSize);
                break;
            
            case 1: //le CLILENT reçoit
                //readFile(request->fileName, buffContent, request->contentSize);
                printf("Contenu envoyé : |%s|\n",contentServeur);
                if(transport->send(to, contentServeur, strlen(contentServeur))) {
                    printf("Le fichier %s a bien été reçu.\n",request->fileName);
                } else {
                    printf("Le fichier %s mal reçu.\n",request->fileName);
                }
                break;               
            
            default : printf("Request error.\n");
        }
    }
}

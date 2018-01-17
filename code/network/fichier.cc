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

char error[] = "ERR_NOT_PRESENT";

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

    if (!fileSystem->Create(fileName, contentSize)) {
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

void ClientFile(int to, float rely) { //ID: 1+
        printf("BIENVENUE DANS LE CLIENT NACHOS\n");
    char buffRequest[MAXREQUESTSIZE];
    char buffContent[MAXFILESIZE];
    //clean
    bzero(buffContent, MAXFILESIZE);
    bzero(buffRequest, MAXREQUESTSIZE);
    FileRequest *request;
    Transport *transport = new Transport(rely);
    
    //creation un fichier
    printf("Création du fichier %s\n", FILENAME);
    createFile((char *)FILENAME, (char *)CONTENT, CONTENTSIZE);
    
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
                if(readFile((char *)FILENAME, buffContent, CONTENTSIZE)) {
                    request = new FileRequest(0, (char *)FILENAME, CONTENTSIZE);
                    createRequest(request, buffRequest);
                    printRequest(request); 
                    if(transport->send(to, buffRequest, strlen(buffRequest))) {
                        printf("La requête a bien été envoyée.\n");
                        printf("Contenu en cours d'envoi : |%s|\n",buffContent);
                        if(transport->send(to, buffContent, strlen(buffContent))) {
                            printf("Le fichier %s a bien été envoyé.\n",request->fileName);
                        } else {
                            printf("Le fichier %s mal été envoyé.\n",request->fileName);
                        }
                    } else {
                        printf("Erreur lors de l'envoie de la requête.\n");
                    }
                        
                } else {
                        printf("La lecture du fichier %s sur votre machine a échoué.\n",FILENAME);
                }

            break;
            
            case '2':
                //demande un fichier au serveur
                printf("Client: demande du fichier %s au serveur\n", FILENAME);
                
                request = new FileRequest(1, (char *)FILENAME);
                createRequest(request, buffRequest);
                printRequest(request);
                if(transport->send(to, buffRequest, strlen(buffRequest))) {
                    printf("La requête a bien été envoyée.\n");
                    transport->viderBoites();
                    if(transport->receive(to, buffContent)) {
                        int ret = strcmp(buffContent,error);
                        if(ret != 0) {
                            printf("Le fichier %s a bien été reçu.\n",request->fileName);
                            printf("Contenu reçu : |%s|\n",buffContent);
                            if(createFile(request->fileName, buffContent, request->contentSize)) {
                                printf("Le fichier %s a bien été créé sur votre machine.\n",request->fileName);
                            } else {
                                printf("Impossible de créer le fichier %s sur votre machine.\n",request->fileName);
                            }
                        } else {
                            printf("La requête n'a pas pu aboutir, le fichier %s n'est pas présent sur le serveur\n",request->fileName);
                        }
                    } else {
                        printf("Erreur lors de la réception du contenu du fichier %s.\n",request->fileName);
                    }
                } else {
                    printf("Erreur lors de l'envoie de la requête.\n");
                }

                break;
            default :
                printf("Requête non reconnue\n\n");
        }
        
        //clean en fin d'iteration
        bzero(buffContent, MAXFILESIZE);
        bzero(buffRequest, MAXREQUESTSIZE);
        transport->viderBoites();
    }
    
}

void ClientText(int to, float rely) { //ID: 1+
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

                printRequest(request); 
                transport->send(to, buffRequest, strlen(buffRequest));

                printf("Contenu en cours d'envoi : |%s|\n",contentClient);
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
                break;
            default :
                printf("Requête non reconnue\n\n");
        }
        
        //clean en fin d'iteration
        bzero(buffContent, MAXFILESIZE);
        bzero(buffRequest, MAXREQUESTSIZE);
        transport->viderBoites();
    }

}

void ServeurFile(int to, float rely) { //ID: 0
                
    char buffRequest[MAXREQUESTSIZE];
    char buffContent[MAXFILESIZE];
    FileRequest *request;
    Transport *transport = new Transport(rely);
    
    while(1) {
        printf("\n --- WAIT REQUEST ---\n\n");
        if(transport->receive(to, buffRequest)) {
            printf("La requête a bien été reçue.\n");
            request = readRequest(buffRequest);
            printRequest(request);
            transport->viderBoites();
            switch(request->type) {
                case 0:  //le CLIENT envoie
                    if(transport->receive(to, buffContent)) {
                        printf("Le contenu du fichier %s a bien été reçu.\n",request->fileName);
                        printf("Contenu reçu: |%s|\n",buffContent);
                        if(createFile(request->fileName, buffContent, request->contentSize)) {
                            printf("Le fichier %s a bien été enregistré sur le serveur.\n",request->fileName);
                        } else {
                            printf("Impossible d'enregistrer le fichier %s sur le serveur.\n",request->fileName);
                        }
                    } else {
                        printf("Erreur lors de la réception du contenu du fichier %s.\n",request->fileName);
                        
                    }

                    break;
                
                case 1: //le CLILENT reçoit
                    if(readFile(request->fileName, buffContent, request->contentSize)) {
                        printf("Contenu en cours d'envoi : |%s|\n",buffContent);
                        if(transport->send(to, buffContent, strlen(buffContent))) {
                            printf("Le fichier %s a bien été reçu.\n",request->fileName);
                        } else {
                            printf("Le fichier %s mal reçu.\n",request->fileName);
                        }
                    } else {
                        printf("Impossible d'envoyer ce fichier.\n");
                        
                        if(transport->send(to, error, strlen(error))) {
                            printf("Le code d'erreur %s a bien été envoyé.\n",error);
                        } else {
                            printf("Le code d'erreur %s n'a pas pu être envoyé.\n",error);
                        }
                    }
                    break;               
                
                default : printf("Request error.\n");
            }
        } else {
            printf("Erreur lors de la réception de la requête.\n");
        }   
    }
    
}

void ServeurText(int to, float rely) { //ID: 0
    char buffRequest[MAXREQUESTSIZE];
    char buffContent[MAXFILESIZE];
    FileRequest *request;
    Transport *transport = new Transport(rely);
    
    char contentServeur[] = "je_suis_le_serveur"; //18
    
    while(1) {
        printf("\n --- WAIT REQUEST ---\n\n");
        transport->receive(to, buffRequest);
        request = readRequest(buffRequest);
        printRequest(request);
        transport->viderBoites();
        switch(request->type) {
            case 0:  //le CLIENT envoie
                if(transport->receive(to, buffContent)) {
                    printf("Le fichier %s a bien été reçu.\n",request->fileName);
                }
                printf("Contenu reçu: |%s|\n",buffContent);
                break;
            
            case 1: //le CLILENT reçoit
                printf("Contenu en cours d'envoi : |%s|\n",contentServeur);
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

#include "serveur.h"
#define SEND 0
#define RECEIVE 1

Serveur(){
    transport = new Transport();
}

~Serveur(){
    delete transport;
}

void Run() {

    char* content;
    int from = 1;

    while(1) {
    
        transport->receive(from, request);
        int type = findTypeRequest(request);
        switch(type) {
            case SEND: {
                char *filename = findFileName(request); 
                OpenFile *openFile = fileSystem->Open(filename);
                bool success = sendFichier(from, openFile);
                printf("Envois du fichier %s %s\n",filename,success?" réussi":" échoué");
            }
            case RECEIVE: {
                char *filename = findFileName(request);
                int filesize = findFileSize(request);
                if (!fileSystem->Create(filename, filesize)) {	 // Create Nachos file
	                printf("Impossible de créer l'OpenFile %s\n", filename);
	                fclose(fp);
	            }
	            OpenFile *openFile = fileSystem->Open(to);
	            receiveFichier(from,openFile);    
                        
            }
            default : printf("Request error.\n");
        }
    
    }

}

bool sendFichier(int to, OpenFile *file){   
    char *content;
    file->Read(content, file->Length());
    
    return transport->send(to, (void*)content,strlen(content));
}


bool receiveFichier(int from, OpenFile *file){
    char *content;
    transport->receive(from, content);
    int size = file->Length();
    return (size == file->Write(content, size));
}

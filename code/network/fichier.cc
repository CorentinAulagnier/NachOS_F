#include "fichier.h"



Fichier(){
    transport = new Transport();
}

~Fichier(){
    delete transport;
}

bool sendFichier(int to, void* content){



    return transport->send(to, (void*)content,sizeof(content));
}


bool receiveFichier(int to, char* name, void* content){

    char* message = "WANT";  
    strcat(message, (const char*)name);
    
    transport->send(to, (void*)message, sizeof(message));


    return transport->receive(int from, void* content);
}

#include "fichier.h"



Fichier(){
    transport = new Transport();
}

~Fichier(){
    delete transport;
}

bool sendFichier(int to){

    char* message = "";  
    
    transport->receive(to, message);
    
    char* name = (char*)((int)message+5);


    void* fichier ;//= ope ...;



    return transport->send(to, fichier, sizeof(fichier));
}


bool receiveFichier(int to, char* name, void* content){

    char* message = "WANT_";  
    strcat(message, (const char*)name);
    
    transport->send(to, (void*)message, sizeof(message));

    return transport->receive(to, content);
}

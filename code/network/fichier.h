#include "system.h"
#include "transport.h"

class Fichier {
    public :
        Fichier();
        ~Fichier();

        bool sendFichier(void* content);
        bool receiveFichier(void* content);
        
    private :
        Transport transport;
};

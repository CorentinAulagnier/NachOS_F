#include "system.h"
#include "transport.h"

class Serveur {
    public :
        Serveur();
        ~Serveur();

        bool sendFichier(void* content);
        bool receiveFichier(void* content);
        
    private :
        Transport transport;
};

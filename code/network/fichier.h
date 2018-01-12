#include "system.h"
#include "transport.h"

class Fichier {
    public :
        Fichier();
        ~Fichier();

        bool sendFichier(int to);
        bool receiveFichier(int to, char* name, void* content);

    private :
        Transport transport;
};

#include "system.h"
#include "transport.h"

class Client {
    public :
        Client();
        ~Client();

        bool sendFichier(void* content);
        bool receiveFichier(void* content);
        
    private :
        Transport transport;
};

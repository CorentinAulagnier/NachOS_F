#include "frameprovider.h"
#include "system.h"

#include  <strings.h>		/* for bzero */

/////////////////////////////////////////
// Constructeur de FrameProvider
/////////////////////////////////////////

FrameProvider::FrameProvider (int nitems)
{
    int timem = time(NULL);
    RandomInit(timem);
    printf("time := %d\n",timem);

    size = nitems;
    bitmap = new BitMap(size);
    lockBitMap = new Lock("LockBitMap");

}

/////////////////////////////////////////
// Destructeur de FrameProvider
/////////////////////////////////////////

FrameProvider::~FrameProvider ()
{

    delete bitmap;
    delete lockBitMap;

}

/////////////////////////////////////////
// Permet d'obtenir un cadre libre
/////////////////////////////////////////

int 
FrameProvider::GetEmptyFrame()
{
    lockBitMap->Acquire();
    int r = Random() % (NumPhysPages);
    int nbTestTotal = size/2 ;
    int nbTest = 0;
    while(bitmap->Test(r) && nbTest < nbTestTotal){
        r = Random() % (NumPhysPages);
        nbTest ++;
    }
    if(nbTest >= nbTestTotal){
        r = bitmap->Find();

    } else {
        bitmap->Mark(r);
    }


    if(r != -1){
        bzero(&(machine->mainMemory[PageSize * r]), PageSize);
    }
    lockBitMap->Release();
    return r;
}

/////////////////////////////////////////
// Permet de relacher un cadre utilisé
/////////////////////////////////////////

void 
FrameProvider::ReleaseFrame(int numFrame)
{
    lockBitMap->Acquire();
    bitmap->Clear(numFrame);
    lockBitMap->Release();    
}

/////////////////////////////////////////
// Donne le nombre restant de cadres libres
/////////////////////////////////////////
 
int
FrameProvider::NumAvailFrame()
{
    lockBitMap->Acquire();
    int retour = bitmap->NumClear();
    lockBitMap->Release();

    return retour;
}

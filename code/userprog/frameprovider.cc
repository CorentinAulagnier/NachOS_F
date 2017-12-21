#include "frameprovider.h"
#include "system.h"

#include  <strings.h>		/* for bzero */

/////////////////////////////////////////
// Constructeur de FrameProvider
/////////////////////////////////////////

FrameProvider::FrameProvider (int nitems)
{
    size = nitems;
    bitmap = new BitMap(size);
    semBitMap = new Semaphore("SemBitMap", 1);

}

/////////////////////////////////////////
// Destructeur de FrameProvider
/////////////////////////////////////////

FrameProvider::~FrameProvider ()
{

    delete bitmap;
    delete semBitMap;

}

/////////////////////////////////////////
// Permet d'obtenir un cadre libre
/////////////////////////////////////////

int 
FrameProvider::GetEmptyFrame()
{
    semBitMap->P();

    RandomInit(time(NULL));
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
    semBitMap->V();

    if(r != -1){
        bzero(&(machine->mainMemory[PageSize * r]), PageSize);
    }

    return r;
}

/////////////////////////////////////////
// Permet de relacher un cadre utilisé
/////////////////////////////////////////

void 
FrameProvider::ReleaseFrame(int numFrame)
{
    semBitMap->P();
    bitmap->Clear(numFrame);
    semBitMap->V();    
}

/////////////////////////////////////////
// Donne le nombre restant de cadres libres
/////////////////////////////////////////
 
int
FrameProvider::NumAvailFrame()
{
    semBitMap->P();
    int retour = bitmap->NumClear();
    semBitMap->V();

    return retour;
}

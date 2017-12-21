#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H

#include "copyright.h"
#include "bitmap.h"
#include "synch.h"
#include "machine.h"


class FrameProvider
{
  public:
    FrameProvider(int nitems);
     ~FrameProvider ();

    int GetEmptyFrame();

    void ReleaseFrame(int numFrame);
    
    int NumAvailFrame();

  private:
    int size;
    BitMap* bitmap;
    Semaphore* semBitMap;
};

#endif

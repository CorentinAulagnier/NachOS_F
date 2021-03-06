#ifndef SYNCHLISTTHREAD_H
#define SYNCHLISTTHREAD_H

#include "copyright.h"
#include "list.h"
#include "synch.h"


typedef struct itemThread{
    int tid;
    Semaphore *semThread;
    itemThread* next;
} itemThread;

itemThread* newItemThread(int tid);

class SynchListThread
{
  public:
    SynchListThread();
    ~SynchListThread ();
    
    void Append (itemThread *);
    itemThread* Remove (int tid);
    itemThread* Find (int tid);
    List* GetList();
    
    Semaphore *lock;
    
  private:
    List * list;

};

#endif // SYNCHLISTTHREAD_H

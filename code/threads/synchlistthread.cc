#include "synchlistthread.h"
#include "userthread.h"

SynchListThread::SynchListThread()
{
    list = new List ();
    lock = new Semaphore ("list lock", 1);
}

SynchListThread::~SynchListThread ()
{
    delete list;
    delete lock;
}

void
SynchListThread::Append (itemThread* item)
{
    lock->P();		

    itemThread * elm = (itemThread *)list->GetFirst();

    if (elm == NULL) {
        list->SetFirst(item);
    } else {
        while (elm->next != NULL ){
            elm = elm->next;
        }
        elm->next = item;
    }
    lock->V();
}

itemThread*
SynchListThread::Remove (int tid)
{
    lock->P();		

    itemThread * elm = (itemThread *)list->GetFirst();
    itemThread * prec = NULL;

    while (elm != NULL && elm->tid != tid) {
        prec = elm;
        elm = elm->next;
    }
    
    if (prec == NULL) {
        elm =  (itemThread *)list->Remove();
    } else if (elm == NULL) {
    } else {
        prec->next = elm->next;
    }

    lock->V();
    return elm;
}

itemThread*
SynchListThread::Find (int tid)
{

    lock->P();		

    itemThread * elm = (itemThread *)list->GetFirst();

    while (elm != NULL && elm->tid != tid) {
        elm = elm->next;
    }
    lock->V();
    return elm;
}

itemThread* newItemThread(int tid) {
    itemThread *it = (itemThread *)malloc(sizeof(itemThread));
    it->tid = tid;
    it->semThread = new Semaphore("semaphore du thread",0);
    it->next = NULL;
    return it;
}


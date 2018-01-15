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
    list->Append(item);
    lock->V();
}

itemThread*
SynchListThread::Remove (int tid)
{
    lock->P();

    ListElement * first = list->GetFirst();
    ListElement * elm = first;
    ListElement * prec = NULL;

    while (elm != NULL && ((itemThread *)elm->item)->tid != tid) {
        prec = elm;
        elm = elm->next;
    }
    
    if (elm == NULL) {
        //rien à enlever
    } else if (prec == NULL) {
        list->SetFirst(first->next);
    } else {
        prec->next = elm->next;
    }

    lock->V();
    return (itemThread *)elm;
}

itemThread*
SynchListThread::Find (int tid)
{

    lock->P();		

    ListElement * elm = list->GetFirst();

    while (elm != NULL && ((itemThread *)elm->item)->tid != tid) {
        elm = elm->next;
    }
    lock->V();
    return (itemThread *)elm->item;
}

itemThread* newItemThread(int tid) {
    itemThread *it = (itemThread *)malloc(sizeof(itemThread));
    it->tid = tid;
    it->semThread = new Semaphore("semaphore du thread",0);
    it->next = NULL;
    return it;
}


List*
SynchListThread::GetList() {
    return this->list;
}

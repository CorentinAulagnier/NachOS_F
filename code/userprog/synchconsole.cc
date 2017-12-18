#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"
#include "console.h"
#include "syscall.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

SynchConsole::SynchConsole(char *readFile, char *writeFile) {

    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
}


SynchConsole::~SynchConsole() {
    delete console;
    delete writeDone;
    delete readAvail;
}

void SynchConsole::SynchPutChar(const char ch) {

    console->PutChar(ch);
    writeDone->P();	// wait for write to finish
      
}

char SynchConsole::SynchGetChar() {

    readAvail->P();	// wait for character to arrive
    return console->GetChar();
    

}

void SynchConsole::SynchPutString(const char s[]) {
    for(unsigned int i=0; s[i] != '\0'; i++)
        SynchPutChar(s[i]);

}


void SynchConsole::SynchGetString(char *s, int n) {
    int i;
    for(i = 0; i<n; i++) {
        s[i] = SynchGetChar();
        if(s[i] == '\n') break;
    }
    s[i] = '\0';
}

void SynchConsole::SynchPutInt(int n){

      char buff[MAX_INT_SIZE];
      snprintf(buff, MAX_INT_SIZE, "%d", n);
      SynchPutString(buff);

}

int SynchConsole::SynchGetInt(){
      int n = 0;
      char buff[MAX_INT_SIZE];
      SynchGetString(buff, MAX_INT_SIZE);
      sscanf(buff, "%d", &n);
      return n;

}

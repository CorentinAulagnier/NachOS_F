#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"
#include "console.h"
#include "syscall.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *synchChar;
static Semaphore *synchChar2;
static Semaphore *synchString;
static Semaphore *synchInt;
static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

SynchConsole::SynchConsole(char *readFile, char *writeFile) {

    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    synchChar = new Semaphore("synchChar", 1);
    synchChar2 = new Semaphore("synchChar2", 1);
    synchString = new Semaphore("synchString", 1);
    synchInt = new Semaphore("synchInt", 1);
    console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
}


SynchConsole::~SynchConsole() {
    delete readAvail;
    delete writeDone;
    delete synchChar;
    delete synchChar2;
    delete synchString;
    delete synchInt;
    delete console;
}

void SynchConsole::SynchPutChar(const char ch) {
    synchChar->P();
printf("Apres prise du verrou\n");
    console->PutChar(ch);
printf("av prise 2nd ver\n");
    writeDone->P();	// wait for write to finish
printf("aPPPP prise 2nd ver\n");
    synchChar->V();
printf("Apres lache du verrou\n");
}

char SynchConsole::SynchGetChar() {
printf("000000000000000\n");
    synchChar2->P();

    readAvail->P();	// wait for character to arrive
    char c = console->GetChar();
printf("AAAAAAAAAAA\n");
    synchChar2->V();
printf("BBBBBBBBBBB\n");
    return c;


}

void SynchConsole::SynchPutString(const char s[]) {
    synchString->P();

    for(unsigned int i=0; s[i] != '\0'; i++){
        SynchPutChar(s[i]);

    }
    synchString->V();
}


void SynchConsole::SynchGetString(char *s, int n) {
    synchString->P();

    int i;
    for(i = 0; i<n; i++) {
        s[i] = SynchGetChar();
        if(s[i] == '\n') break;
    }
    s[i] = '\0';

    synchString->V();
}

void SynchConsole::SynchPutInt(int n){
    synchInt->P();

    char buff[MAX_INT_SIZE];
    snprintf(buff, MAX_INT_SIZE, "%d", n);
    SynchPutString(buff);

    synchInt->V();
}

int SynchConsole::SynchGetInt(){
    synchInt->P();

    int n = 0;
    char buff[MAX_INT_SIZE];
    SynchGetString(buff, MAX_INT_SIZE);
    sscanf(buff, "%d", &n);

    synchInt->V();
    return n;
}

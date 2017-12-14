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

char* buffer;
int positionBuffer;

SynchConsole::SynchConsole(char *readFile, char *writeFile) {

    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
    buffer = (char*)malloc(MAX_STRING_SIZE);
    positionBuffer = 0;
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
    char c = console->GetChar();
    //if(c == EOF)
        //printf(" c2zdqqzdzqd:  %c\n",c);
    return c;
    

}

void SynchConsole::vider(char* buff, int size){
    for(int i = 0; i<size; i++)
        buff[i] = '\0';
}

void SynchConsole::ecrire(char* buff, int size){
    for(int j = 0; j<size && buff[j]!='\0'; j++)
        SynchPutChar(buff[j]);
    vider(buff, size);
    positionBuffer = 0;
}

void SynchConsole::SynchPutString(const char s[]) {
    int i = 0;
    
    while(s[i] != '\0') {
        // Buffer plein, on l'ecrit et on le vide
        if (positionBuffer == MAX_STRING_SIZE) {
            ecrire(buffer, MAX_STRING_SIZE);
        }
        // Ajout du s dans le buffer lettre par lettre
        buffer[positionBuffer] = s[i];
        positionBuffer++;
        i++;
    }
    // Ecriture termine, on ecrit et on vide le buffer
    ecrire(buffer, MAX_STRING_SIZE);
    printf("\n put = %s\n",s);
}


void SynchConsole::SynchGetString(char *s, int n) {
    

    int i;char  c;
    
    for(i = 0; i<n; i++) {
        c = SynchGetChar();
            printf(" c :  %c\n",c);
        s[i] = c;
    }

   // s[i] = '\0';
    
    printf(" get :  %s\n",s);
    
}

#include "copyright.h"
#include "system.h"
#include "synchconsole.h"
#include "synch.h"
#include "console.h"
#include "syscall.h"

static Semaphore *readAvail;
static Semaphore *writeDone;
static Semaphore *lecture;
static Semaphore *ecriture;
static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

SynchConsole::SynchConsole(char *readFile, char *writeFile) {

    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    lecture = new Semaphore("lecture", 1);
    ecriture = new Semaphore("ecriture", 1);
    console = new Console(readFile, writeFile, ReadAvail, WriteDone, 0);
}

SynchConsole::~SynchConsole() {
    delete readAvail;
    delete writeDone;
    delete lecture;
    delete ecriture;
    delete console;
}

//////////////////////////////////////////////////////
//WriteChar(char ch) & ReadChar()
// Fonctions de base pour les lectures et les ecritures
// unitaire
//////////////////////////////////////////////////////

void SynchConsole::WriteChar(char ch){
    console->PutChar(ch);
    writeDone->P();	// wait for write to finish
}

char SynchConsole::ReadChar(){
    readAvail->P();	// wait for character to arrive
    return console->GetChar();
}

//////////////////////////////////////////////////////
//LectureChaine(char *s, int n) & EcritureChaine(char *s)
// Fonctions de base pour les lectures et les ecritures
// de plusieurs caracteres
//////////////////////////////////////////////////////

void SynchConsole::LectureChaine(char *s, int n) {
    int i;
    for(i = 0; i<n; i++) {
        s[i] = ReadChar();
        if(s[i] == '\n') break;
    }
    s[i] = '\0';

}

void SynchConsole::EcritureChaine(char *s) {
    for(unsigned int i=0; s[i] != '\0'; i++){
        WriteChar(s[i]);
    }
}

//////////////////////////////////////////////////////
//SynchPutChar(const char ch) & SynchGetChar()
// Fonctions de lecture et d'ecriture d'un char
// Synchronisees
//////////////////////////////////////////////////////

void SynchConsole::SynchPutChar(const char ch) {
    ecriture->P();
    
    WriteChar(ch);
    
    ecriture->V();
}

char SynchConsole::SynchGetChar() {
    lecture->P();

    char c = ReadChar();

    lecture->V();
    return c;
}

//////////////////////////////////////////////////////
//SynchPutString(const char s[]) & SynchGetString(char *s, int n)
// Fonctions de lecture et d'ecriture d'un string
// Synchronisees
//////////////////////////////////////////////////////

void SynchConsole::SynchPutString(const char s[]) {
    ecriture->P();
    
    EcritureChaine((char*)s);
    
    ecriture->V();
}

void SynchConsole::SynchGetString(char *s, int n) {
    lecture->P();

    LectureChaine(s, n);

    lecture->V();
}

//////////////////////////////////////////////////////
//SynchPutInt(int n) & SynchGetInt()
// Fonctions de lecture et d'ecriture d'un int
// Synchronisees
//////////////////////////////////////////////////////

void SynchConsole::SynchPutInt(int n){
    ecriture->P();

    char buff[MAX_INT_SIZE];
    snprintf(buff, MAX_INT_SIZE, "%d", n);
    EcritureChaine(buff);
    
    ecriture->V();
}

int SynchConsole::SynchGetInt(){
    lecture->P();

    int n = 0;
    char buff[MAX_INT_SIZE];
    
    LectureChaine(buff, MAX_INT_SIZE);
    sscanf(buff, "%d", &n);

    lecture->V();
    return n;
}

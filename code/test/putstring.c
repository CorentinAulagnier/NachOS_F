#include "syscall.h"

void test() {

    PutString("Quelle taille voulez-vous lire ?\n"); 

    int taille = 5;
    PutInt(taille);
    PutChar('\n');
    
    taille = GetInt();
    
    PutInt(taille);
    PutChar('\n');
    
    PutString("Entrez un mot\n");
    char s[taille];
    GetString(s,taille);
    
    PutString("Mot entré : \n");
    PutString(s);
    PutChar('\n');
    
    char c = 'a';
    PutChar(c);
    PutChar('\n');
    
    c = GetChar();
    PutChar(c);
    PutChar('\n');
    
}

int main() {
    test();
    Halt();
}

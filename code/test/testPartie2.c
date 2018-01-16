#include "syscall.h"

void testPutChar() {
    PutString("\n- Lancement testPutChar -\n"); 
    char c;
    c = 'a';PutChar(c);
    c = 'z';PutChar(c);
    c = 'A';PutChar(c);
    c = 'Z';PutChar(c);
    c = '1';PutChar(c);
    c = ' ';PutChar(c);
    c = '9';PutChar(c);
    c = '\n';PutChar(c);
    c = '\t';PutChar(c);
    c = '!';PutChar(c);
    c = '%';PutChar(c);
    c = '\0';PutChar(c);
    c = '$';PutChar(c);
    PutString("\n- Fin testPutChar -\n");
}

void ajout(char s[], char* c){
    int i = 0;
    for(i=0; c[i] != '\0'; i++){
        s[i] = c[i];
    }
    s[i] = '\0';
}

void testPutString() {
    PutString("\n- Lancement testPutString -\n");
    char s[50];
    ajout(s, " ");PutString(s);
    ajout(s, "\n");PutString(s);
    ajout(s, "");PutString(s);
    ajout(s, "Yo"); PutString(s);
    ajout(s, "Hello");PutString(s);
    ajout(s, "Bonjour");PutString(s);
    ajout(s, "Bonjour\0");PutString(s);
    ajout(s, "Bonjour\n");PutString(s);
    ajout(s, "Bonjour\t");PutString(s);
    ajout(s, "Bon\0jour");PutString(s);
    ajout(s, "Bon\njour");PutString(s);
    ajout(s, "Bon\tjour");PutString(s);
    ajout(s, "\0Bonjour");PutString(s);
    ajout(s, "\nBonjour");PutString(s);
    ajout(s, "\tBonjour");PutString(s);
    ajout(s, "Bonjour!Bonjour?Bonjour!Bonjour?\n");PutString(s);
    ajout(s, ",?;.:/!§ù%*µ£");PutString(s);
    ajout(s, "&é'(-è_çà)=");PutString(s);
    ajout(s, "~1234567890°+");PutString(s);
    ajout(s, "¬¹~#{[|`^@]}");PutString(s);
    PutString("\n- Fin testPutString -\n");
}
void testPutInt() {
    PutString("\n- Lancement testPutInt -\n");
    int i = 0;PutInt(i);
    i = 1;PutInt(i);
    i = 2;PutInt(i);
    i = 3;PutInt(i);
    i = 9;PutInt(i);PutChar('\n');
    i = 10;PutInt(i);
    i = 11;PutInt(i);
    i = 20;PutInt(i);
    i = 22;PutInt(i);
    i = 30;PutInt(i);
    i = 33;PutInt(i);
    i = 99;PutInt(i);PutChar('\n');
    i = 100;PutInt(i);
    i = 101;PutInt(i);
    i = 399;PutInt(i);
    i = 599;PutInt(i);
    i = 999;PutInt(i);PutChar('\n');
    i = 1000;PutInt(i);
    i = 1001;PutInt(i);
    i = 1111;PutInt(i);
    i = 9999;PutInt(i);
    PutString("\n- Fin testPutInt -\n");
}
void testGetChar() {
    PutString("\n- Lancement testGetChar -\n");
    PutString("\n- Ecrivez un caractere, il sera reecrit apres ! -\n");
    PutString("\n- Ecrivez 'Q' pour quitter-\n");
    char c = 'a';
    
    while(c != 'Q') {
        c = GetChar();PutChar(c);PutChar('\n');
    }
    PutString("\n- Fin testGetChar -\n");
}

int cmp(char s[], char* c){
    int i = 0;
    for(i=0; c[i] != '\0'; i++){
        if (s[i] != c[i]) return 0;
    }
    if (s[i] != '\0') return 0;
    return 1;
}

void testGetString() {
    PutString("\n- Lancement testGetString -\n");
    PutString("\n- Ecrivez une chaine de caracteres, elle sera reecrite apres ! -\n");
    PutString("\n- Ecrivez 'exit' pour quitter-\n");
    char c[50];
    
    while(!cmp(c, "exit")) {
        GetString(c, 50);PutString(c);PutChar('\n');
    }
    PutString("\n- Fin testGetString -\n");
}
void testGetInt() {
    PutString("\n- Lancement testGetInt -\n");
    PutString("\n- Ecrivez un entier, elle sera reecrite apres ! -\n");
    PutString("\n- Ecrivez '666' pour quitter-\n");
    int c = 0;
    
    while(c != 666) {
        c = GetInt();PutInt(c);PutChar('\n');
    }
    PutString("\n- Fin testGetInt -\n");
} 


int main() {
    char c = '1';
    while (c != '0') {
        PutString("Quel test voulez vous lancer ?\n");
        PutString("\t 1 - PutChar\n");
        PutString("\t 2 - PutString\n");
        PutString("\t 3 - PutInt\n");
        PutString("\t 4 - GetChar\n");
        PutString("\t 5 - GetString\n");
        PutString("\t 6 - GetInt\n");
        PutString("\t 0 - Exit\n");
        c = GetChar();
        if(c=='\n') {
            c = GetChar();
        }
        switch (c) {
        case '1':
            testPutChar();
        break;
        case '2':
            testPutString();
        break;
        case '3':
            testPutInt();
        break;
        case '4':
            testGetChar();
        break;
        case '5':
            testGetString();
        break;
        case '6':
            testGetInt();
        break;
        default :
            PutString("Test non reconnu\n\n");
        }
    }
    Halt();
}

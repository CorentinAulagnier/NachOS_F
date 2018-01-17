#include "syscall.h"

void f1(void* c) {
    PutString("\nSortie de f1:\n");
    PutChar(*(char*)c);
    PutChar('\n');
    UserThreadExit();
}

void f2(void* i){
    PutString("\nSortie de f2:\n");
    PutInt(*(int*)i);
    PutChar('\n');
    UserThreadExit();
}

void f3(void* s){
    PutString("\nSortie de f3:\n");
    PutString((char*)s);
    PutChar('\n');
    UserThreadExit();
}

void f4(void* max) {
    int i, count = 0;
    for(i=0; i<(*(int*)max); i++) {
        count += i;
        count -= i;
        count *= 33;
        count /= 33;
    }
    PutString("\nSortie de f4:\n");
    PutInt(count);
    PutChar('\n');
    UserThreadExit();
}

void test_simult() {
    PutString("***** Test n°1: Threads simultanés *****\n");
    PutString("2 threads lancés simultanément puis récupérés:");

    int i = 45;
    char c = 'A';
    int tidt1f1 = UserThreadCreate(&f1, &c);
    int tidt1f2 = UserThreadCreate(&f2, &i);

    PutString("\ntid associé au thread exécutant f1 = ");
    PutInt(tidt1f1);
    PutString("\ntid associé au thread exécutant f2 = ");
    PutInt(tidt1f2);

    UserThreadJoin(tidt1f1);
    UserThreadJoin(tidt1f2);
    
    PutString("\n***** Fin Test n°1: Threads simultanés *****\n\n");
}

void test_seq() {
    PutString("***** Test n°2: Threads séquentiels *****\n");
    PutString("2 threads lancés puis récupérés séquentiellement:\n");

    int i = 6582;
    int tidt2f2 = UserThreadCreate(&f2, &i);
    PutString("\ntid associé au thread exécutant f2 = ");
    PutInt(tidt2f2);
    UserThreadJoin(tidt2f2);
    
    char* s = "hello world";
    int tidt2f3 = UserThreadCreate(&f3, s);
    PutString("\ntid associé au thread exécutant f3 = ");
    PutInt(tidt2f3);
    UserThreadJoin(tidt2f3);
    PutString("\n***** Fin Test n°2: Threads séquentiels *****\n\n");
}

void test_bcp() {
    PutString("***** Test n°3: Threads très occupés  *****\n");
    PutString("2 threads lancés avec une fonction très longue à exécuter:\n");

    int i = 100000;
    
    int t1 = UserThreadCreate(&f4, &i);
    int t2 = UserThreadCreate(&f4, &i);
    
    PutString("\ntid associé au thread exécutant f4 = ");
    PutInt(t1);
    PutString("\ntid associé au thread exécutant f4 = ");
    PutInt(t2);
    
    UserThreadJoin(t1);
    UserThreadJoin(t2);
    
    PutString("\n***** Fin Test n°3: Threads très occupés *****\n\n");

}

void test_trop_de_threads() {
    PutString("***** Test n°4: Dépassement nombres de threads *****\n");
    PutString("100 threads lancés simultanément, gestion du dépassement du nombre maximum de threads");
    PutString(" (5-1 pour le main)\n");
    int i;
    int nbthread = 100;
    int n[nbthread];
    for( i = 0; i<nbthread; i++) {
        n[i] = 10 + i;
    }
    
    int tids[nbthread];
    
    //vérification de la réussite de la création des threads
    for(i = 0; i<nbthread; i++) {
        tids[i] = UserThreadCreate(&f2, &n[i]);
        if(tids[i] == 0) {
            PutString("La création du ");
            PutInt(i+1);
            PutString(" ème thread a échoué.");
            PutString("\n");
        } else {
            PutString("tid associé au ");
            PutInt(i+1);
            PutString(" ème thread exécutant f2 = ");
            PutInt(tids[i]);
            PutString("\n");
        }
    }

    //récupération des threads se terminant
    for(i = 0; i<nbthread; i++) {
        if(tids[i] != 0) {
            UserThreadJoin(tids[i]);
        }
    }
    
    PutString("\n***** Fin Test n°4: Dépassement nombres de threads *****\n\n");
}

void test_robustesse() {
    PutString("***** Test n°5: Robustesse *****\n");
    
    PutString("Tentative de UserThreadExit hors d'un thread:");
    UserThreadExit();
    
    PutString("\n\nTentative de UserThreadJoin sur thread inexistant:");
    UserThreadJoin(5);
    
    PutString("\n***** Fin Test n°5: Robustesse *****\n\n");
}

int main() {
    while (1) {
        PutString("Quel test voulez vous lancer ?\n");
        PutString("\t 1 - Threads simultanés\n");
        PutString("\t 2 - Threads séquentiels\n");
        PutString("\t 3 - Beaucoup d'opérations\n");
        PutString("\t 4 - Dépacement nombres de threads\n");
        PutString("\t 5 - Robustesse\n");
        PutString("\t 0 - Exit\n");
        char c = GetChar();
        if(c=='\n') {
            c = GetChar();
        }
        switch (c) {
            case '0':
                PutString("\n***** Fin des tests ******\n\n");   
                Halt();
                break;
            case '1':
                test_simult();
                break;
            case '2':
                test_seq();
                break;
            case '3':
                test_bcp();
                break;
            case '4':
                test_trop_de_threads();
                break;
            case '5':
                test_robustesse();
                break;
            default :
                PutString("Test non reconnu\n\n");
        }
    }
}

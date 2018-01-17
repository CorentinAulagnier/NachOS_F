// main.cc 
//      Bootstrap code to initialize the operating system kernel.
//
//      Allows direct calls into internal operating system functions,
//      to simplify debugging and testing.  In practice, the
//      bootstrap code would just initialize data structures,
//      and start a user program to print the login prompt.
//
//      Most of this file is not needed until later assignments.
//
// Usage: nachos -d <debugflags> -rs <random seed #>
//              -s -x <nachos file> -c <consoleIn> <consoleOut>
//              -f -cp <unix file> <nachos file>
//              -p <nachos file> -r <nachos file> -l -D -t
//              -n <network reliability> -m <machine id>
//              -o <other machine id>
//              -z
//
//    -d causes certain debugging messages to be printed (cf. utility.h)
//    -rs causes Yield to occur at random (but repeatable) spots
//    -z prints the copyright message
//
//  USER_PROGRAM
//    -s causes user programs to be executed in single-step mode
//    -x runs a user program
//    -c tests the console
//
//  FILESYS
//    -f causes the physical disk to be formatted
//    -cp copies a file from UNIX to Nachos
//    -p prints a Nachos file to stdout
//    -r removes a Nachos file from the file system
//    -l lists the contents of the Nachos directory
//    -D prints the contents of the entire file system 
//    -t tests the performance of the Nachos file system
//
//  NETWORK
//    -n sets the network reliability
//    -m sets this machine's host id (needed for the network)
//    -o runs a simple test of the Nachos network software
//
//  NOTE -- flags are ignored until the relevant assignment.
//  Some of the flags are interpreted here; some in system.cc.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#define MAIN
#include "copyright.h"
#undef MAIN

#include "utility.h"
#include "system.h"


// External functions used by this file

extern void ThreadTest (void), Copy (const char *unixFile, const char *nachosFile);
extern void Print (char *file), PerformanceTest (void);
extern void StartProcess (char *file), ConsoleTest (char *in, char *out);
extern void SynchConsoleTest (char *in, char *out);
extern void MailTest (int networkID);
extern void Boucle (int farAddr);
extern void EnvoiTest(int farAddr, float reli);
extern void ReceptionTest (int farAddr, float reli);
extern void testRequest();
extern void ClientText(int to, float rely);
extern void ServeurText(int to, float rely);
extern void ClientFile(int to, float rely);
extern void ServeurFile(int to, float rely);
extern void CreateDir(const char *file), RemoveFile(const char *file);
extern void FsList(void), PrintAll(void);
extern void CreateFile(const char* file, int size), MoveToDir(const char *file);
extern void RmDir(const char *file);
//----------------------------------------------------------------------
// main
//      Bootstrap the operating system kernel.  
//      
//      Check command line arguments
//      Initialize data structures
//      (optionally) Call test procedure
//
//      "argc" is the number of command line arguments (including the name
//              of the command) -- ex: "nachos -d +" -> argc = 3 
//      "argv" is an array of strings, one for each command line argument
//              ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------

int
main (int argc, char **argv)
{

    int argCount;		// the number of arguments 
    // for a particular command

    DEBUG ('t', "Entering main");
    (void) Initialize (argc, argv);
#ifdef THREADS
    ThreadTest ();
#endif

    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount)
      {
	  argCount = 1;
	  if (!strcmp (*argv, "-z"))	// print copyright
	      printf ("%s", copyright);
#ifdef USER_PROGRAM
	  if (!strcmp (*argv, "-x"))
	    {			// run a user program
		ASSERT (argc > 1);
		StartProcess (*(argv + 1));
		argCount = 2;
	    }
	  else if (!strcmp (*argv, "-c"))
	    {			// test the console
		if (argc == 1)
		    ConsoleTest (NULL, NULL);
		else
		  {
		      ASSERT (argc > 2);
		      ConsoleTest(*(argv + 1), *(argv + 2));
		      argCount = 3;
		  }
		interrupt->Halt ();	// once we start the console, then 
		// Nachos will loop forever waiting 
		// for console input
	    }
        	  else if (!strcmp (*argv, "-sc"))
	    {
            if (argc == 1)
		    SynchConsoleTest(NULL, NULL);
		else
		  {
		      ASSERT (argc > 2);
		      SynchConsoleTest(*(argv + 1), *(argv + 2));
		      argCount = 3;
		  }
		interrupt->Halt ();
          }
#endif // USER_PROGRAM
#ifdef FILESYS
	  if (!strcmp (*argv, "-testfile")) {
    char buffer[60];
        while(1) {
          fgets(buffer,60,stdin);

        char ** args  = NULL;
        char *  p    = strtok (buffer, " ");
        int n_spaces = 0;


        /* split string and append tokens to 'res' */

        while (p) {
          args = (char**)realloc (args, sizeof (char*) * ++n_spaces);

          if (args == NULL)
            exit (-1); /* memory allocation failed */

          args[n_spaces-1] = p;

          p = strtok (NULL, " ");
        }

        /* realloc one extra element for the last NULL */

        args = (char**)realloc (args, sizeof (char*) * (n_spaces+1));
        args[n_spaces] = 0;

        char *pos;
        if ((pos=strchr(args[n_spaces-1], '\n')) != NULL)
        *pos = '\0';


/*for (i = 0; i < (n_spaces+1); ++i)
  printf ("res[%d] = %s\n", i, args[i]); */

	      if (!strcmp(args[0],"mkdir")) {
                CreateDir(args[1]);
            } else if (!strcmp(args[0],"cp")) {
			    Copy (args[1],args[2]);	
		    } else if (!strcmp(args[0],"p")) {
			    Print (args[1]);
		    } else if (!strcmp(args[0],"r")) {
                RemoveFile(args[1]);
		    } else if (!strcmp(args[0],"l")) {
                FsList();
		    } else if (!strcmp(args[0],"D")) {
                PrintAll();
		    } else if (!strcmp(args[0],"t")) {
                PerformanceTest();
		    } else if (!strcmp(args[0],"mkfile")) {
                CreateFile(args[1], atoi(args[2]));
		    } else if (!strcmp(args[0],"cd")) {
                MoveToDir(args[1]);
		    } else if (!strcmp(args[0],"rmdir")) {
                RmDir(args[1]);
		    }

        }
    }
#endif // FILESYS
#ifdef NETWORK
	  if (!strcmp (*argv, "-o"))
	    {
		ASSERT (argc > 1);
		Delay (2);	// delay for 2 seconds
		// to give the user time to 
		// start up another nachos
		
        char* c = *(argv + 2);
        DEBUG ('w',"c = %c\n",c[0]);
        
        float rely = atof(*(argv + 4));
        if (rely == 0) rely = 1;
                
        if (c[0] == 't') { /* Test mail simple */
		    MailTest (atoi (*(argv + 1)));
        
        } else if (c[0] == 'm') { /* Test Protocole transport */

                if ((atoi (*(argv - 1))) == 0) { // Machine 0
                    EnvoiTest (atoi (*(argv + 1)), rely);
                    
                } else if ((atoi (*(argv - 1))) == 1) { // Machine 1
                    ReceptionTest (atoi (*(argv + 1)), rely);
                }
        
        } else if (c[0] == 'z') { /* Test Protocole fichier version text */

                if ((atoi (*(argv - 1))) == 0) {
                    ServeurText (atoi (*(argv + 1)),rely);
                } else if ((atoi (*(argv - 1))) == 1) {
                    ClientText (atoi (*(argv + 1)),rely);
                }
        
        } else if (c[0] == 'f') { /* Test Protocole fichier version file*/

                if ((atoi (*(argv - 1))) == 0) {
                    ServeurFile (atoi (*(argv + 1)),rely);
                } else if ((atoi (*(argv - 1))) == 1) {
                    ClientFile (atoi (*(argv + 1)),rely);
                }
        
        } else { // erreur
            printf("Test network non reconnu\n");
        }

		argCount = 2;
	    }
#endif // NETWORK
      }

    currentThread->Finish ();	// NOTE: if the procedure "main" 
    // returns, then the program "nachos"
    // will exit (as any other normal program
    // would).  But there may be other
    // threads on the ready list.  We switch
    // to those threads by saying that the
    // "main" thread is finished, preventing
    // it from returning.
    return (0);			// Not reached...
}

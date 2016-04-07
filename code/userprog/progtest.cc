// progtest.cc 
//	Test routines for demonstrating that Nachos can load
//	a user program and execute it.  
//
//	Also, routines for testing the Console hardware device.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "console.h"
#include "addrspace.h"
#include "synch.h"

//----------------------------------------------------------------------
// StartProcess
// 	Run a user program.  Open the executable, load it into
//	memory, and jump to it.
//----------------------------------------------------------------------
void execFunc2(int null)
{
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();		// load page table register
    machine->Run();
    ASSERT(false);
}
void
StartProcess(char *filename)
{   
    pageBitMap->printFit(customFitArg);
	 printf("Selected: ");
	 if (customVArg == 2)
		printf("FIFO\n");
	 else if (customVArg == 3)
		printf("Random\n");
	 else printf("Demand paging\n");
	if (extraOutput)
		printf("Extra -E output option selected.\n");

    Thread *main = new Thread("Main thread");
    OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL) {
	printf("Unable to open file %s\n", filename);
	return;
    }

    printf("\nLoading Process %d with path %s\n", currentThread->getThisThreadID(), filename);
    
    space = new AddrSpace(executable);    
    printf("LOADING FILE %s\n", filename);
    space->SetFileName(filename);
    main->space = space;

    delete executable;			// close file
    
    pcb->append(main);

    main->Fork(execFunc2, NULL);
					// the address space exits
					// by doing the syscall "exit"
}

// Data structures needed for the console test.  Threads making
// I/O requests wait on a Semaphore to delay until the I/O completes.

static Console *console;
static Semaphore *readAvail;
static Semaphore *writeDone;

//----------------------------------------------------------------------
// ConsoleInterruptHandlers
// 	Wake up the thread that requested the I/O.
//----------------------------------------------------------------------

static void ReadAvail(int arg) { readAvail->V(); }
static void WriteDone(int arg) { writeDone->V(); }

//----------------------------------------------------------------------
// ConsoleTest
// 	Test the console by echoing characters typed at the input onto
//	the output.  Stop when the user types a 'q'.
//----------------------------------------------------------------------

void 
ConsoleTest (char *in, char *out)
{
    char ch;

    console = new Console(in, out, ReadAvail, WriteDone, 0);
    readAvail = new Semaphore("read avail", 0);
    writeDone = new Semaphore("write done", 0);
    
    for (;;) {
	readAvail->P();		// wait for character to arrive
	ch = console->GetChar();
	console->PutChar(ch);	// echo it!
	writeDone->P() ;        // wait for write to finish
	if (ch == 'q') return;  // if q, quit
    }
}

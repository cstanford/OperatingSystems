// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include <stdio.h>        // FA98
#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "addrspace.h"   // FA98
#include "synch.h"
#include "sysdep.h"   // FA98
#include "list.h"

// begin FA98
static Semaphore consoleSem("Protects read/write", 1);
static int SRead(int addr, int size, int id);
static void SWrite(char *buffer, int size, int id);

SpaceId SExec(int filename);
void execFunc(int filename);
int joinFunc(int child);
char* readString(int addr);
extern BitMap* pageBitMap;
void exitFunc();

List pcbList;	//list to keep track of parent-children processessss

// end FA98

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    int arg1 = machine->ReadRegister(4);
    int arg2 = machine->ReadRegister(5);
    int arg3 = machine->ReadRegister(6);
    int Result;
    int i, j;
    int proof = currentThread->getThisThreadID();
    //printf("----CURRENT-THREAD-%d----\n", proof);
    char *ch = new char [500];

    switch ( which )
    {
    case NoException :
	break;
    case SyscallException :

    // for debugging, in case we are jumping into lala-land
    // Advance program counters.
    machine->registers[PrevPCReg] = machine->registers[PCReg];
    machine->registers[PCReg] = machine->registers[NextPCReg];
    machine->registers[NextPCReg] = machine->registers[NextPCReg] + 4;

    switch ( type )
    {

    case SC_Halt :
	DEBUG('t', "Shutdown, initiated by user program.\n");
    printf("HALTING\n");
	interrupt->Halt();
	break;

	    // FIXED ERRROR IN SC_READ!
    case SC_Read :
	if (arg2 <= 0 || arg3 < 0){
		printf("\nRead 0 byte.\n");
		break; // BONUS POINT BUG FIX :)
	}
	Result = SRead(arg1, arg2, arg3);
	machine->WriteRegister(2, Result);
	DEBUG('t',"Read %d bytes from the open file(OpenFileId is %d)",
	arg2, arg3);
	break;

    case SC_Write :
	for (j = 0; ; j++) {
	    if(!machine->ReadMem((arg1+j), 1, &i))
		j=j-1;
	    else{
		ch[j] = (char) i;
		if (ch[j] == '\0') 
		    break;
	    }
	}
	if (j == 0){
	    printf("\nWrite 0 byte.\n");
	    // SExit(1);
	} else {
	    DEBUG('t', "\nWrite %d bytes from %s to the open file(OpenFileId is %d).", arg2, ch, arg3);
	    SWrite(ch, j, arg3);
	}
	break;

    case SC_Exec :
     
	machine->WriteRegister(2, SExec(arg1));
	break;

    case SC_Exit :
	exitFunc();
	break;

    case SC_Join :
	joinFunc(arg1);
//	machine->WriteRegister(2, JoinFunc(arg1));
	break;

    case SC_Yield :
    printf("----PERFORMED-SC-YIELD----\n");
	currentThread->Yield();
	break;

    default :
	//Unprogrammed system calls end up here
	break;
    }         
    break;

    case ReadOnlyException :
	puts ("ReadOnlyException");
	if (currentThread->getName() == "main")
	ASSERT(FALSE);  //Not the way of handling an exception.
	//SExit(1);
	break;
    case BusErrorException :
	puts ("BusErrorException");
	if (currentThread->getName() == "main")
	ASSERT(FALSE);  //Not the way of handling an exception.
	//SExit(1);
	break;
    case AddressErrorException :
	puts ("AddressErrorException");
	if (currentThread->getName() == "main")
	ASSERT(FALSE);  //Not the way of handling an exception.
	//SExit(1);
	break;
    case OverflowException :
	puts ("OverflowException");
	if (currentThread->getName() == "main")
	ASSERT(FALSE);  //Not the way of handling an exception.
	//SExit(1);
	break;
    case IllegalInstrException :
	puts ("IllegalInstrException");
	if (currentThread->getName() == "main")
	ASSERT(FALSE);  //Not the way of handling an exception.
	//SExit(1);
	break;
    case NumExceptionTypes :
	puts ("NumExceptionTypes");
	if (currentThread->getName() == "main")
	ASSERT(FALSE);  //Not the way of handling an exception.
	//SExit(1);
	break;

	default :
	//      printf("Unexpected user mode exception %d %d\n", which, type);
	//      if (currentThread->getName() == "main")
	//      ASSERT(FALSE);
	//      SExit(1);
	break;
    }
    delete [] ch;
}

char* readString(int addr){
    char str[500];
    int i;
    int j;
    for(j = 0; ; j++){
	if(!machine->ReadMem((addr+j), 1, &i))
	    j = j-1;
	else {
	    str[j] = (char)i;
	    if(str[j] == '\0')
		break;
	}
    }
    return str;
}

static int SRead(int addr, int size, int id)  //input 0  output 1
{
    char buffer[size+10];
    int num,Result;

	//read from keyboard, try writing your own code using console class.
    if (id == 0)
    {
	scanf("%s",buffer);

	num=strlen(buffer);
	if(num>(size+1)) {

		buffer[size+1] = '\0';
		Result = size+1;
	}
	else {
		buffer[num+1]='\0';
		Result = num + 1;
	}

	for (num=0; num<Result; num++)
	{
	    machine->WriteMem((addr+num), 1, (int) buffer[num]);
	    if (buffer[num] == '\0')
		break;
	 }
	return num;

    }
    //read from a unix file, later you need change to nachos file system.
    else
    {
	for(num=0;num<size;num++){
	    Read(id,&buffer[num],1);
	    machine->WriteMem((addr+num), 1, (int) buffer[num]);
	    if(buffer[num]=='\0') break;
	}
	return num;
    }
}



static void SWrite(char *buffer, int size, int id)
{
    //write to terminal, try writting your own code using console class.
    if (id == 1){
        consoleSem.P();
        printf("%s", buffer);
        consoleSem.V();
    }
    //write to a unix file, later you need change to nachos file system.
    if (id >= 2)
	WriteFile(id,buffer,size);
}

SpaceId SExec(int filename)
{
    char* name = readString(filename);
    int id = currentThread->getThisThreadID();
    printf("Thread %d performed syscall SC_EXEC on file  %s\n", id,  name);
    //Create a new thread to do this stuff
    OpenFile *executable = fileSystem->Open(name);
    AddrSpace *space;

    Thread *userProg = new Thread(name);
    
    
    if (executable == NULL) {
	printf("Unable to open file %s\n", name);
	return -1;
    }
    space = new AddrSpace(executable);    
    userProg->space = space;

    delete executable;			// close file

    //userProg->space->InitRegisters();		// set the initial register values
    //userProg->space->RestoreState();		// load page table register

//	pcbList.append
    userProg->Fork(execFunc, (int)name);
    userProg->setParentID(currentThread->getThisThreadID());
    userProg->setParentThread(currentThread);

    return userProg->getThisThreadID();

}

void execFunc(int filename)
{
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();		// load page table register
    printf("CURRENTLY RUNNING THREAD %d", currentThread->getThisThreadID());
    machine->Run();
    //ASSERT(false);
}

void exitFunc()
{
    if(currentThread->getParentThread() != NULL)
	if(currentThread->getParentThread()->getWaitingID() == currentThread->getThisThreadID())
	    currentThread->getParentThread()->joinSem->V();
    // Check if thread has parent.
    // if it does, wake up parent.

    TranslationEntry *pageTable;
    
    pageTable = currentThread->space->getPageTable();
    int numPages = currentThread->space->getNumPages();

    currentThread->space->ClearMemory();

    int id = currentThread->getThisThreadID();

    printf("\n Thread %did has exited\n", id);
    
    
    currentThread->Finish();    

}
int joinFunc(int child){
    currentThread->setWaitingID(child);
    currentThread->joinSem->P();
	return 0;
}

// end FA98


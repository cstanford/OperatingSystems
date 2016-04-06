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
#include "noff.h"
// begin FA98
static Semaphore pcbSem("Don't touch my pcbList yo", 1);
static int SRead(int addr, int size, int id);
static void SWrite(char *buffer, int size, int id);

SpaceId SExec(int filename);
void execFunc(int filename);
int joinFunc(int child);
char* readString(int addr);
extern BitMap* pageBitMap;
void exitFunc(int exitStatus);

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

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
    //All stuff for handling page faults
    int badAddr;
    int pageToLoad;
    char* name;
    OpenFile *executable;

    //Regular exception handling variables
    int type = machine->ReadRegister(2);

    int arg1 = machine->ReadRegister(4);
    int arg2 = machine->ReadRegister(5);
    int arg3 = machine->ReadRegister(6);
    int Result;
    int i, j;
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
	exitFunc(arg1);
	break;

    case SC_Join :
	machine->WriteRegister(2, joinFunc(arg1));
	break;

    case SC_Yield :
	printf("\n--Thread %d PERFORMED SC_YIELD--\n", currentThread->getThisThreadID());
	currentThread->Yield();
	break;

    default :
	//Unprogrammed system calls end up here
	printf("\n--Thread %d PERFORMED AN UNPROGRAMMED SYSCALL\n", currentThread->getThisThreadID());
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
	printf("Pointer out of bounds. Terminating thread %d.\n", currentThread->getThisThreadID());
	exitFunc(-1);	
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
	exitFunc(-1);	
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
    case PageFaultException:
    //Failing virtual address read is in register 39
    badAddr = machine->ReadRegister(39);
    //Get the name of the executable
    name = currentThread->space->GetFileName();
    //Read this in from the executable
    //executable = fileSystem->Open(name);
    currentThread->space->ResolvePageFault(badAddr);
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
        printf("%s", buffer);
    }
    //write to a unix file, later you need change to nachos file system.
    if (id >= 2)
	WriteFile(id,buffer,size);
}

SpaceId SExec(int filename)
{
    char* name = readString(filename);
    char fileName[100];
    strcpy(fileName, name);
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
    NoffHeader noffH;
    unsigned int i, size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    
    if (noffH.noffMagic != NOFFMAGIC) 
    {
	printf("Wrong file type. Unable to load file.\n\n");
	delete executable;			// close file
	return -1;
    }

    space = new AddrSpace(executable);    
    space->SetFileName(fileName);
    userProg->space = space;
    pcbSem.P();
    pcb->append(userProg);
    pcbSem.V();
    delete executable;			// close file

    userProg->Fork(execFunc, (int)fileName);
    userProg->setParentID(currentThread->getThisThreadID());
    userProg->setParentThread(currentThread);

    return userProg->getThisThreadID();

}

void execFunc(int filename)
{
    currentThread->space->InitRegisters();
    currentThread->space->RestoreState();		// load page table register
    machine->Run();
    //ASSERT(false);
}

void exitFunc(int exitStatus)
{
    printf("\n--Thread %d PERFORMED SC_EXIT--\n", currentThread->getThisThreadID());

    TranslationEntry *pageTable;
    
    pageTable = currentThread->space->getPageTable();
    int numPages = currentThread->space->getNumPages();

    currentThread->space->ClearMemory();

    int id = currentThread->getThisThreadID();
    if(currentThread->getParentThread() != NULL){
	if(currentThread->getParentThread()->getWaitingID() == currentThread->getThisThreadID()){
	    currentThread->getParentThread()->setJoinExitStatus(exitStatus);
	    currentThread->getParentThread()->joinSem->V();
	}
    }
    // Check if thread has parent.
    // if it does, wake up parent.

    pcbSem.P();
    pcb->remove(currentThread);
    pcbSem.V();
    printf("EXIT STATUS %d\n", exitStatus);
    currentThread->Finish();    

}
int joinFunc(int child){
    printf("\n--Thread %d PERFORMED SC_JOIN--\n", currentThread->getThisThreadID());
    pcbSem.P();
    if(pcb->isValidID(child)){
	currentThread->setWaitingID(child);
	pcbSem.V();
	currentThread->joinSem->P();
	return currentThread->getJoinExitStatus();//return the exiting chid's exec value
    } else {
	pcbSem.V();
	return -1;
    }
}

// end FA98


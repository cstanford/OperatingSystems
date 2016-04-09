// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "list.h"
//#include "noff.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

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

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    unsigned int i, size;

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    
    if (noffH.noffMagic != NOFFMAGIC) 
    {
	printf("Wrong file type. Unable to load file.\n\n");
	//do other stuff
	if(currentThread->getParentThread() != NULL)
	{
	    if(currentThread->getParentThread()->getWaitingID() == currentThread->getThisThreadID())
		currentThread->getParentThread()->joinSem->V();
	}

	currentThread->Finish();    
    }
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize);
    size = numPages * PageSize;

    //ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);


// first, set up the translation 
    bitMapSem.P();
    int avail = pageBitMap->FindFit(numPages, customFitArg);
    /*
    if (avail == -1) 
    {
	printf("Not enough memory;\nPages full.\n\n");
	//do other stuff
	if(currentThread->getParentThread() != NULL)
	    if(currentThread->getParentThread()->getWaitingID() == currentThread->getThisThreadID())
		currentThread->getParentThread()->joinSem->V(); 
	currentThread->Finish();    
    }
/*
    printf("\nPage availability before adding the process:\n");
    pageBitMap->Print();
    printf("Num pages: %d\n", numPages);
    printf("Fits at index: %d\n", avail);
 */   
    placementTable = new int[numPages];
    pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
        placementTable[i] = LOADBINARY; //Everything has to be loaded from executable initially
        //pageBitMap->Mark(i+avail);
        pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
        //pageTable[i].physicalPage = i+avail;
        pageTable[i].valid = FALSE;
        pageTable[i].use = FALSE;
        pageTable[i].dirty = FALSE;
        pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
                        // a separate page, we could set its 
                        // pages to be read-only
        //bzero(&(machine->mainMemory[PageSize*(avail+i)]), PageSize );
    }
  //  printf("\nPage availability after adding the process:\n");
   // pageBitMap->Print();

    bitMapSem.V();
    //Create swapfile
    globalThreadIDSem.P();
    int swapID = globalThreadID;
    sprintf(swapFileName, "%d.swap", swapID); 
    globalThreadIDSem.V();
    fileSystem->Create(swapFileName, size);
    swapFile = fileSystem->Open(swapFileName);

    for(int i = 0; i < size; i++)
    {
	    swapFile->WriteAt("0",1,i);
    }
    for(int i = 0; i < size; i++){
        char *t;
        //Memory in code segment
        if(InCode(i)){
            executable->ReadAt(t,
                1, noffH.code.inFileAddr + i - noffH.code.virtualAddr);
            swapFile->WriteAt(t, 1, i);
        }
        //Memory in init segment
        else if(InData(i)){
            executable->ReadAt(t,
                1, noffH.initData.inFileAddr + i - noffH.initData.virtualAddr);
            swapFile->WriteAt(t, 1, i);
        }
    }
/* 
// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    //bzero(machine->mainMemory, size);
    for(int i=0; i<numPages; i++)
    {
        bzero( &(machine->mainMemory[pageTable[i].physicalPage * PageSize]), PageSize );
    }
// then, copy in the code and data segments into memory
    int physAddr = pageTable[noffH.code.virtualAddr/PageSize].physicalPage*PageSize + (noffH.code.virtualAddr%PageSize);
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[physAddr]),
			noffH.code.size, noffH.code.inFileAddr);
    }
     physAddr = pageTable[noffH.initData.virtualAddr/PageSize].physicalPage*PageSize + (noffH.initData.virtualAddr%PageSize);
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[physAddr]),
			noffH.initData.size, noffH.initData.inFileAddr);
    }
//*/
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    delete executable;
    delete swapFile;
    delete [] pageTable;
    delete [] placementTable;
}
void AddrSpace::ClearMemory(){
    int index;
    bitMapSem.P();	
    printf("\nPage availability before exiting the process:\n");
    pageBitMap->Print();
    for(int i = 0; i < numPages; i++)
    {
        index = pageTable[i].physicalPage;
        if(pageTable[i].valid)
            pageBitMap->Clear(index);
    }
    printf("\nPage availability after exiting the process:\n");
    pageBitMap->Print();
    bitMapSem.V();
    printf("Deleting %s\n", swapFileName);
    fileSystem->Remove(swapFileName);
}
//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{ 
    pageTable = machine->pageTable;
    numPages = machine->pageTableSize;
}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}

int AddrSpace::getNumPages()
{
    return numPages;
}

TranslationEntry * AddrSpace::getPageTable()
{
    return pageTable;
}

void AddrSpace::SetFileName(char* filename){
    this->filename = filename;
    ASSERT(executable = fileSystem->Open(filename));

}
char* AddrSpace::GetFileName(){
    return filename;
}
void AddrSpace::ResolvePageFault(int badVAddr){ 
    int pageToLoad = (badVAddr)/PageSize;
    SwapIn(pageToLoad);
    //LoadPage(pageToLoad);
}

//Maybe unneeded?
void AddrSpace::LoadPage(int pageNum){
    /*switch(placementTable[pageNum])
    {
	case LOADBINARY:
	{
        //Load from the executable
        LoadFromExec(pageNum);
	    break;
	}

	case LOADSWAP:
	{
        //Load from swap
	    break;
	}

	default:
	    break; 
    }*/

}

bool AddrSpace::InCode(int addr){
    if(addr >= noffH.code.virtualAddr && addr < noffH.code.virtualAddr + noffH.code.size){
        return true;
    }
    return false;
}

bool AddrSpace::InData(int addr){
    if(addr >= noffH.initData.virtualAddr && addr < noffH.initData.virtualAddr + noffH.initData.size){
        return true;
    }
    return false;
}

int AddrSpace::Translate(int vAddr){
    int virtualPage = vAddr/PageSize;
    int offset = vAddr%PageSize;
    int physAddr = pageTable[virtualPage].physicalPage*PageSize + offset;
    return physAddr;
}

void AddrSpace::SwapIn(int pageToLoad){

    //Claim the resource
    bitMapSem.P();
    //Find a frame for it
    printf("\nPage availability before handling page fault:\n");
    pageBitMap->Print();
    pageTable[pageToLoad].physicalPage = pageBitMap->Find(); //Run page replacement algo
    int physPage = pageTable[pageToLoad].physicalPage;
    hashSem.P();
    if(pageTable[pageToLoad].physicalPage == -1){

        if(customVArg == 1){
            //FIFO Page replacement goes here
            int i = *(int*)fifoList->Remove();
            ipt[i].threadP->space->SwapOut(ipt[i].vpn, ipt[i].threadP->getThisThreadID());
            pageTable[pageToLoad].physicalPage = i; //Run page replacement algo
            pageBitMap->Mark(i);
            physPage = pageTable[pageToLoad].physicalPage;
        } else if(customVArg == 2){
            //Random Page replacement goes here
            //
            //This is just a test for swap-out
            int i = Random() % NumPhysPages;
            /*ASSERT(ipt[i].threadP);
            ASSERT(ipt[i].threadP != NULL);
            ASSERT(ipt[i].threadP->getThisThreadID() != NULL);*/
            ipt[i].threadP->space->SwapOut(ipt[i].vpn, ipt[i].threadP->getThisThreadID());
            pageTable[pageToLoad].physicalPage = i; //Run page replacement algo
            pageBitMap->Mark(i);
            physPage = pageTable[pageToLoad].physicalPage;
        } else {
            bitMapSem.V();
            printf("Sorry, there is not enough memory. Terminating thread.\n");
            ClearMemory();
            currentThread->Finish();
        }
    }
    //Set the valid bit to troo
    pageTable[pageToLoad].valid = TRUE;
    ipt[physPage].threadP = &(*currentThread);
    ipt[physPage].vpn = pageToLoad;
    IPTHash t;
    t.pid = currentThread->getThisThreadID();
    t.vpn = pageToLoad;
    if(extraOutput){
        printf("Process %d requests VPN %d\n", currentThread->getThisThreadID(), pageToLoad);
        printf("Assigning Frame %d\n", physPage);
    }
    hmap.put(t, physPage);
    if(customVArg == 1){
        fifoList->Append(new int(physPage));
    } 
    int val = -1;
    hashSem.V();
    //placementTable[pageToLoad] = LOADED;
    printf("\nPage availability after handling page fault:\n");
    pageBitMap->Print();
    printf("Current Thread is %d\n", currentThread->getThisThreadID());
    printf("Num pages: %d\n", numPages);
    printf("Fits at index: %d\n", pageTable[pageToLoad].physicalPage);
    printf("Loading virtual page: %d\n", pageToLoad);
    bitMapSem.V();
    
    
    for(int i = pageToLoad*PageSize; i < (pageToLoad+1)*PageSize; i++){
        int physAddr = Translate(i);
        swapFile->ReadAt(&(machine->mainMemory[physAddr]), 1, i);
    }
}

//Takes in virtual page to write out, and which thread PID owns it, since currentThread is not guaranteed to be the owner
void AddrSpace::SwapOut(int pageToWrite, int pid){
    for(int i = pageToWrite*PageSize; i < (pageToWrite+1)*PageSize; i++){
        int physAddr = Translate(i);
        swapFile->WriteAt(&(machine->mainMemory[physAddr]), 1, i);
    }
    pageTable[pageToWrite].valid = FALSE;
    pageBitMap->Clear(pageTable[pageToWrite].physicalPage);
    IPTHash t;
    t.pid = pid; 
    t.vpn = pageToWrite;
    hmap.remove(t);
    if(extraOutput){
        printf("Swapped out PID %d VPN %d Frame %d\n", t.pid, t.vpn, pageTable[pageToWrite].physicalPage);
    }
}

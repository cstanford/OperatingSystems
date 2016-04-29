// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the 
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector, 
//
//      Unlike in a real system, we do not keep track of file permissions, 
//	ownership, last modification date, etc., in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "filehdr.h"

#define DirectLimit 15872
#define IndirectLimit 32000
#define MaxPointers (SectorSize/sizeof(int))

//----------------------------------------------------------------------
// FileHeader::Allocate
// 	Initialize a fresh file header for a newly created file.
//	Allocate data blocks for the file out of the map of free disk blocks.
//	Return FALSE if there are not enough free blocks to accomodate
//	the new file.
//
//	"freeMap" is the bit map of free disk sectors
//	"fileSize" is the bit map of free disk sectors
//----------------------------------------------------------------------

bool
FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
    numBytes = fileSize;
    numSectors  = divRoundUp(fileSize, SectorSize);
    if (freeMap->NumClear() < numSectors)
        return FALSE;		// not enough space
    if(numBytes <= DirectLimit){
        for (int i = 0; i < numSectors; i++)
            dataSectors[i] = freeMap->Find();
    }else if(numBytes > DirectLimit &&
            numBytes <= IndirectLimit){
        for (int i = 0; i < NumDirect ; i++)
            dataSectors[i] = freeMap->Find();
        int remainingSectors = numSectors - (NumDirect-1);
        printf("Number of Sectors is: %d\n", numSectors);
        printf("Remaining Sectors is: %d\n", remainingSectors);
        printf("Indirect pointer stores value of %d\n", dataSectors[NumDirect-1]);
        int extraSectors[MaxPointers]; //MaxPointers

        for (int i = 0; i < remainingSectors; i++){
            extraSectors[i] = freeMap->Find();
            printf("Found a sector at %d\n", extraSectors[i]);
        }
        synchDisk->WriteSector(dataSectors[NumDirect-1], (char *)extraSectors); //Write indirect pointer data
        int *data = new int[MaxPointers];
        synchDisk->ReadSector(dataSectors[NumDirect-1], (char*)data);

        for (int i = 0; i < remainingSectors; i++){
            printf("Sector stored correctly with value of %d\n", data[i]);
        }

    }else {
        for (int i = 0; i < NumDirect ; i++)
            dataSectors[i] = freeMap->Find();
        int remainingSectors = numSectors - (NumDirect-2);
        printf("Number of Sectors is: %d\n", numSectors);
        printf("Remaining Sectors is: %d\n", remainingSectors);
        printf("Indirect pointer stores value of %d\n", dataSectors[NumDirect-2]);
        
        //Takes care of our indirect pointer, which is full
        int extraSectors[MaxPointers]; //MaxPointers

        for (int i = 0; i < MaxPointers; i++){
            extraSectors[i] = freeMap->Find();
            printf("Found a sector at %d\n", extraSectors[i]);
        }
        synchDisk->WriteSector(dataSectors[NumDirect-2], (char *)extraSectors); //Write indirect pointer data

        printf("Double Indirect pointer stores value of %d\n", dataSectors[NumDirect-1]);
        int remainingIndirect = remainingSectors-MaxPointers;
        //Now we need to handle the doubly-indirect pointer 
        int indirectNeeded = remainingIndirect/MaxPointers;
        int indirectPointers[MaxPointers];
        //Round up to handle excess
        if((remainingIndirect%MaxPointers) != 0)
            indirectNeeded += 1;
        for(int i = 0; i < indirectNeeded; i++){
            //Allocate space for each indirect pointer
            indirectPointers[i] = freeMap->Find();
            printf("Found a sector at %d\n", indirectPointers[i]);
        }
        //Write that list of indirect pointers to disk

        synchDisk->WriteSector(dataSectors[NumDirect-1], (char *)indirectPointers); 
        printf("Just wrote indirect pointers to disk\n");
        printf("Now we need to allocate for each one's direct pointers\n");
        printf("Indirect needed is %d\n", indirectNeeded);
        //Now we need to write the data for each indirect pointer
        for(int i = 0; i < indirectNeeded; i++){
            //Write the sectors for the ith pointer
            if(remainingIndirect < MaxPointers){
                //Handle rest of sectors
                int rSectors[MaxPointers];
                for(int r = 0; r < remainingIndirect; r++){
                    rSectors[r] = freeMap->Find(); 
                    printf("Found a sector at %d\n", rSectors[r]);
                }
                synchDisk->WriteSector(indirectPointers[i], (char *)rSectors); 
                printf("Just wrote indirect %dths direct pointers\n");
            }
            else {
                //This indirect pointer has MaxPointers sectors to write
                int rSectors[MaxPointers];
                for(int r = 0; r < MaxPointers; r++){
                    rSectors[r] = freeMap->Find(); 
                    printf("Found a sector at %d\n", rSectors[r]);
                }
                synchDisk->WriteSector(indirectPointers[i], (char *)rSectors); 
                remainingIndirect -= MaxPointers;
            }
        }
    }
    return TRUE;
}

//----------------------------------------------------------------------
// FileHeader::Deallocate
// 	De-allocate all the space allocated for data blocks for this file.
//
//	"freeMap" is the bit map of free disk sectors
//----------------------------------------------------------------------

//todo
void 
FileHeader::Deallocate(BitMap *freeMap)
{
    /*for (int i = 0; i < numSectors; i++) {
	ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
	freeMap->Clear((int) dataSectors[i]);
    }*/
    freeMap->Print();
    if(numBytes <= DirectLimit){
        for (int i = 0; i < numSectors; i++){
            ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
            freeMap->Clear((int) dataSectors[i]);
        }
    }else if(numBytes > DirectLimit &&
            numBytes <= IndirectLimit){
        for (int i = 0; i < NumDirect ; i++){
            ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
            freeMap->Clear((int) dataSectors[i]);
        }
        int remainingSectors = numSectors - (NumDirect-1);
        int extraSectors[MaxPointers];

        int *data = new int[MaxPointers];
        synchDisk->ReadSector(dataSectors[NumDirect-1], (char*)data);

        for (int i = 0; i < remainingSectors; i++){
            ASSERT(freeMap->Test((int) data[i]));  // ought to be marked!
            freeMap->Clear((int) data[i]);
        }

    }else {
        for (int i = 0; i < NumDirect ; i++){
            ASSERT(freeMap->Test((int) dataSectors[i]));  // ought to be marked!
            freeMap->Clear((int) dataSectors[i]);
        }
        int remainingSectors = numSectors - (NumDirect-2);
        
        //Takes care of our indirect pointer, which is full
        int *data = new int[MaxPointers];
        synchDisk->ReadSector(dataSectors[NumDirect-2], (char*)data);

        for (int i = 0; i < MaxPointers; i++){
            ASSERT(freeMap->Test((int) data[i]));  // ought to be marked!
            freeMap->Clear((int) data[i]);
        }

        int remainingIndirect = remainingSectors-(MaxPointers);
        //Now we need to handle the doubly-indirect pointer 
        int indirectNeeded = remainingIndirect/(MaxPointers);

        int *indirectPointers = new int[MaxPointers];
        //Round up to handle excess
        if((remainingIndirect%(MaxPointers)) != 0){
            indirectNeeded += 1;
        }

        synchDisk->ReadSector(dataSectors[NumDirect-1], (char *)indirectPointers); 
        //Now we need to write the data for each indirect pointer
        for(int i = 0; i < indirectNeeded; i++){
            //Write the sectors for the ith pointer
            if(remainingIndirect < MaxPointers){
                int *data = new int[MaxPointers];
                synchDisk->ReadSector(indirectPointers[i], (char*)data);

                for (int i = 0; i < remainingIndirect; i++){
                    ASSERT(freeMap->Test((int) data[i]));  // ought to be marked!
                    freeMap->Clear((int) data[i]);
                }
            }
            else {
                int *data = new int[MaxPointers];
                synchDisk->ReadSector(indirectPointers[i], (char*)data);

                for (int i = 0; i < (MaxPointers); i++){
                    ASSERT(freeMap->Test((int) data[i]));  // ought to be marked!
                    freeMap->Clear((int) data[i]);
                }
                remainingIndirect -= MaxPointers;
            }

            ASSERT(freeMap->Test((int) indirectPointers[i]));  // ought to be marked!
            freeMap->Clear((int) indirectPointers[i]);
        }
    }
    freeMap->Print();
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::FetchFrom(int sector)
{
    synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::WriteBack(int sector)
{
    synchDisk->WriteSector(sector, (char *)this); 
}

//----------------------------------------------------------------------
// FileHeader::ByteToSector
// 	Return which disk sector is storing a particular byte within the file.
//      This is essentially a translation from a virtual address (the
//	offset in the file) to a physical address (the sector where the
//	data at the offset is stored).
//
//	"offset" is the location within the file of the byte in question
//----------------------------------------------------------------------

int
FileHeader::ByteToSector(int offset)
{
    if(numBytes <= DirectLimit){
        return dataSectors[offset / SectorSize];
    }else if(numBytes > DirectLimit &&
            numBytes <= IndirectLimit){
        if(offset/SectorSize < (NumDirect-1)){
            return dataSectors[offset / SectorSize];
        }
        else{
            int *data = new int[MaxPointers];
            synchDisk->ReadSector(dataSectors[NumDirect-1], (char*)data);
            return data[offset/SectorSize - (NumDirect-1)];
        }
    }else {
        if(offset/SectorSize < (NumDirect-2)){
            return dataSectors[offset / SectorSize];
        }
        else if(offset/SectorSize >= (NumDirect-2) &&
                (offset/SectorSize < MaxPointers+NumDirect-2)) {
            //printf("Called byte to sector\n");
            int *data = new int[MaxPointers];
            synchDisk->ReadSector(dataSectors[NumDirect-2], (char*)data);
            int directSector = offset/SectorSize - (NumDirect-2);
            //printf("Looking for the sector %d in the indirect pointer\n", directSector);
            //printf("Think the data is stored at %d\n", data[directSector]);
            return data[directSector];
        } else {
            //Handle double indirect translation
            int *indirectPointers= new int[MaxPointers];
            synchDisk->ReadSector(dataSectors[NumDirect-1], (char*)indirectPointers);
            //Which sector do we want out of the MaxPointers^2 stored by
            //the doubly indirect pointer?
            int whichDirectSector = (offset/SectorSize - (NumDirect-2) - MaxPointers);
            //printf("We're looking for sector %d in DL\n", whichDirectSector);
            //To which indirect pointer does that sector belong?
            int whichIndirect = (whichDirectSector)/MaxPointers;
            //printf("We think it is in the %dth indirect sector\n", whichIndirect);
            //printf("That indirect points us to sector %d\n", indirectPointers[whichIndirect]);
            //What is the local offset for that sector
            //in the indirect pointer's MaxPointers possible direct pointers?
            int directOffset = whichDirectSector%MaxPointers;

            //Load that indirect pointer's data here
            int *directPointers = new int[MaxPointers];
            synchDisk->ReadSector(indirectPointers[whichIndirect], (char*)directPointers);
            //Return the sector pointed to by the directOffset
            return directPointers[directOffset];
        }
    }
}

//----------------------------------------------------------------------
// FileHeader::FileLength
// 	Return the number of bytes in the file.
//----------------------------------------------------------------------

int
FileHeader::FileLength()
{
    return numBytes;
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
    int i, j, k;
    char *data = new char[SectorSize];

    printf("FileHeader contents.  File size: %d.  File blocks:\n", numBytes);
    for (i = 0; i < numSectors; i++)
	printf("%d ", dataSectors[i]);
    printf("\nFile contents:\n");
    for (i = k = 0; i < numSectors; i++) {
	synchDisk->ReadSector(dataSectors[i], data);
        for (j = 0; (j < SectorSize) && (k < numBytes); j++, k++) {
	    if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
		printf("%c", data[j]);
            else
		printf("\\%x", (unsigned char)data[j]);
	}
        printf("\n"); 
    }
    delete [] data;
}

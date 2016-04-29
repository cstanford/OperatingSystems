// bitmap.c 
//	Routines to manage a bitmap -- an array of bits each of which
//	can be either on or off.  Represented as an array of integers.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "bitmap.h"

//----------------------------------------------------------------------
// BitMap::BitMap
// 	Initialize a bitmap with "nitems" bits, so that every bit is clear.
//	it can be added somewhere on a list.
//
//	"nitems" is the number of bits in the bitmap.
//----------------------------------------------------------------------

BitMap::BitMap(int nitems) 
{ 
    numBits = nitems;
    numWords = divRoundUp(numBits, BitsInWord);
    map = new unsigned int[numWords];
    for (int i = 0; i < numBits; i++) 
        Clear(i);
}

//----------------------------------------------------------------------
// BitMap::~BitMap
// 	De-allocate a bitmap.
//----------------------------------------------------------------------

BitMap::~BitMap()
{ 
    delete map;
}

//----------------------------------------------------------------------
// BitMap::Set
// 	Set the "nth" bit in a bitmap.
//
//	"which" is the number of the bit to be set.
//----------------------------------------------------------------------

void
BitMap::Mark(int which) 
{ 
    ASSERT(which >= 0 && which < numBits);
    map[which / BitsInWord] |= 1 << (which % BitsInWord);
}
    
//----------------------------------------------------------------------
// BitMap::Clear
// 	Clear the "nth" bit in a bitmap.
//
//	"which" is the number of the bit to be cleared.
//----------------------------------------------------------------------

void 
BitMap::Clear(int which) 
{
    ASSERT(which >= 0 && which < numBits);
    map[which / BitsInWord] &= ~(1 << (which % BitsInWord));
}

//----------------------------------------------------------------------
// BitMap::Test
// 	Return TRUE if the "nth" bit is set.
//
//	"which" is the number of the bit to be tested.
//----------------------------------------------------------------------

bool 
BitMap::Test(int which)
{
    ASSERT(which >= 0 && which < numBits);
    
    if (map[which / BitsInWord] & (1 << (which % BitsInWord)))
	return TRUE;
    else
	return FALSE;
}

//----------------------------------------------------------------------
// BitMap::Find
// 	Return the number of the first bit which is clear.
//	As a side effect, set the bit (mark it as in use).
//	(In other words, find and allocate a bit.)
//
//	If no bits are clear, return -1.
//----------------------------------------------------------------------

int 
BitMap::Find() 
{
    for (int i = 0; i < numBits; i++)
	if (!Test(i)) {
	    Mark(i);
	    return i;
	}
    return -1;
}

//----------------------------------------------------------------------
// BitMap::NumClear
// 	Return the number of clear bits in the bitmap.
//	(In other words, how many bits are unallocated?)
//----------------------------------------------------------------------

int 
BitMap::NumClear() 
{
    int count = 0;

    for (int i = 0; i < numBits; i++)
	if (!Test(i)) count++;
    return count;
}

//----------------------------------------------------------------------
// BitMap::Print
// 	Print the contents of the bitmap, for debugging.
//
//	Could be done in a number of ways, but we just print the #'s of
//	all the bits that are set in the bitmap.
//----------------------------------------------------------------------

void
BitMap::Print() 
{
    printf("[");
    int count = 0;
    for (int i = 0; i < numBits; i++){
	if(Test(i)) count++;
	if(i==numBits-1)
	    printf("%d", Test(i));
	else
	    printf("%d ", Test(i));
    }
    printf("] %d%%", (int)((float)(100.0f*count)/(float)numBits));
    printf("\n"); 
}
void
BitMap::UglyPrint() 
{
    for (int i = 0; i < numBits; i++){
	    if(Test(i))
            printf("%d ", i);
    }
    printf("\n"); 
}

// These aren't needed until the FILESYS assignment

//----------------------------------------------------------------------
// BitMap::FetchFromFile
// 	Initialize the contents of a bitmap from a Nachos file.
//
//	"file" is the place to read the bitmap from
//----------------------------------------------------------------------

void
BitMap::FetchFrom(OpenFile *file) 
{
    file->ReadAt((char *)map, numWords * sizeof(unsigned), 0);
}

//----------------------------------------------------------------------
// BitMap::WriteBack
// 	Store the contents of a bitmap to a Nachos file.
//
//	"file" is the place to write the bitmap to
//----------------------------------------------------------------------

void
BitMap::WriteBack(OpenFile *file)
{
   file->WriteAt((char *)map, numWords * sizeof(unsigned), 0);
}

// Main Course //
// code by Krista Zabicki :) 

// ~~~~~~~~~~~~~~~~~
// FindFit
//	Given a size, passes the size to the appropriate fit statement
//		that is passed in command line
//
int BitMap::FindFit(int size, int fitArg)
{
	int index = -1;
	switch (fitArg) {
		case 3:
			index = WorstFit(size);
			break;
		case 2:
			index = BestFit(size);
			break;
		default:
			index = FirstFit(size);
	}
	return index;
}

// ~~~~~~~~~~~~~~~~~
// FirstFit
//	Given a size, finds the first location in the BitMap that can accomodate
//	that size.
//
int BitMap::FirstFit(int size)
{
	int index = -1;
	bool free = true;
	for(int i = 0; i < numBits; i++) // loop through the bitmap
	{
		if (!Test(i)) {					// if i-th bit is not set, found a free block of data
			free = true;					// set free to true
			for (int j = i; j < i+size; j++) {	// loop from i to where the block should end
				if (j >= numBits || Test(j)) {				// if a block has a bit set
					free = false;			// then not a free block to use
					i = j;					// continue bitmap for-loop from this index
					break;					// break from this loop
				}
			}
			if (free) {			// if the block is free
				index = i;			// index is i
				break;				// break from loop
			}
		}
	}
	return index;
}

// ~~~~~~~~~~~~~~~~~
// BestFit
//	Given a size, finds the best location in the BitMap that can accomodate
//	that size.
// 
int BitMap::BestFit(int size)
{
	int index = -1;
	int gap = 0;
	int currentindex = 0;
	int bestgap = numBits+1;
	for(int i = 0; i < numBits; i++)		// loop through bitmap
	{
		if (!Test(i)) {
			gap = 0;
			currentindex = i;
			while (!Test(i)) 
			{
				gap += 1;
				i += 1;
				if (i >= numBits) break;
			} 
			if (gap < bestgap && gap >= size) {
				bestgap = gap;
				index = currentindex;
			}
		}
	}
	return index;
}

// ~~~~~~~~~~~~~~~~~
// WorstFit
//	Given a size, finds the worst location in the BitMap that can accomodate
//	that size.
// 
int BitMap::WorstFit(int size)
{
	int index = -1;
	int gap = 0;
	int currentindex = 0;
	int worstgap = 0;
	for(int i = 0; i < numBits; i++)
	{
		if (!Test(i)) {
			gap = 0;
			currentindex = i;
			while (!Test(i))
			{
				gap += 1;
				i += 1;
				if (i >= numBits) break;
			}
			if (gap > worstgap && gap >= size){
				worstgap = gap;
				index = currentindex;
			}
		}
	}
	return index;
}

void BitMap::printFit(int fitNum)
{
    if (fitNum == 3)
	printf("\nSelected: Worst Fit\n");
    else if (fitNum == 2)
	printf("\nSelected: Best Fit\n");
    else
	printf("\nSelected: First Fit\n");
 
}
// End Main course

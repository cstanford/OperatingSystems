#include "system.h"
#include "string.h"
#include "shout.h"
#include "inputidentification.h"
#include "diningphilosophers.h"


//----------------------------------------------------------------------
// SimpleThread
//  Loop 5 times, yielding the CPU to another ready thread 
//  each iteration.
//
//  "which" is simply a number identifying the thread, for debugging
//  purposes.
//----------------------------------------------------------------------


void
SimpleThread(int which)
{
    int num;

    for (num = 0; num < 5; num++) {
    printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest
//  Invoke a test routine.
//----------------------------------------------------------------------

// edited by Connor Stanford and Joseph Fontenot

void
ThreadTest()
{
    DEBUG('t', "Entering ThreadTest");

    /********** Previous Code ****************

      Thread *t = new Thread("forked thread");

      t->Fork(SimpleThread, 1);
      SimpleThread(0);
      
     *****************************************/

    // This thread checks for valid input
    if (customParameterArg == 1){
        Thread *checkInput = new Thread("InputIdentificationThread");
        checkInput->Fork(InputIdentification, 0);
    }

    // This thread calls the shoutItOut function
    // 0 is a fake parameter to satisfy the requirements to fork a thread.
    else if (customParameterArg == 2){
        Thread *shout = new Thread("ShoutItOutThread");
        shout->Fork(ShoutItOut, 0);
    }
    else if (customParameterArg == 3){
        Thread *dine = new Thread("DiningPhilosophersThread");
        dine->Fork(DiningPhilosophers, 0);
    }
    else{
	printf("Sorry, please use the paramters \"-A \" followed by an integer value 1 - 6.\n\n");
   } 
}


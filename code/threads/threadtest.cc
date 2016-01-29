#include "copyright.h"
#include "system.h"
#include "string.h"
#include "shout.h"
#include "inputidentification.h"

//----------------------------------------------------------------------
// SimpleThread
//  Loop 5 times, yielding the CPU to another ready thread 
//  each iteration.
//
//  "which" is simply a number identifying the thread, for debugging
//  purposes.
//----------------------------------------------------------------------

//TODO: check for proper input on task 2

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
    else{
        printf("\nIncorrect input. Please use \"-A 1\" or \"-A 2\"\n\n");
    }   


}



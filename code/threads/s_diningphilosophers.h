#ifndef DINING_PHILOS_S_
#define DINING_PHILOS_S_

#include "system.h"
#include "synch.h"
#include "inputidentification.h"
#include "philosopher_globals.h"
#include "s_philosopher.h"

Semaphore sit("sit", 0);
Semaphore sitAux("sitAux", 0);
Semaphore wait("Politeness", 1);
Semaphore waitAux("ExtraPolite", 0);

// Added to test lock
Lock lockwait("so polite");
Lock locksit("just wow");

int sitCount = 0;
int finishCount = 0;
 // dummyParameters is only a placeholder to satisy the required parameters 
 // when forking a thread.
void S_BeginDining(int dummyParameter) 
{
    S_Philosopher phil(numOfPhilosophers, numOfMeals, s_chopsticks);
    s_philosopherArrayPointer[philosopherIdNumber] = phil;
    phil.setId(philosopherIdNumber);
    philosopherIdNumber++;
    phil.join();

//    sit.P();
    locksit.Acquire();
    sitCount++;
    phil.sit();
//    sit.V();
	 locksit.Release();

    if(sitCount == numOfPhilosophers)
        sitAux.V();
    sitAux.P();
    sitAux.V();

    // Continue eating until all meals are gone.
    do {
        phil.beginEating();
    }while(numOfMeals > 0);

    //wait.P();
	 lockwait.Acquire();

    finishCount++;
    printf("Philosopher %d is waiting to leave...(%d Philosophers Remaining)\n", phil.getId(), numOfPhilosophers-finishCount);
    if(finishCount == numOfPhilosophers){
        printf("All philosophers leave the table together.\n\n");
        waitAux.V();
    }
    //wait.V();
	 lockwait.Release();

    waitAux.P();
    waitAux.V();

}


void S_DiningPhilosophers(int dummyParameters)
 // dummyParameters is only a placeholder to satisy the required parameters 
 // when forking a thread.
{
    char userInput[256];
    input_type type;

    do{ // gets the number of philosophers from the user.
        printf("\nEnter the number of philosophers (1 - 500): ");
        getInput(userInput, 255);
        type = CheckType(userInput);
        numOfPhilosophers = atoi(userInput);
        if(type != INTEGER){
            printf("Sorry, only positive integer values are allowed.\n");
        }
	if(numOfPhilosophers <= 1)
	{
	    printf("Sorry, please enter a value greater than 1.\n");
	    type = DECIMAL;
	    //forces a loop back.
	}
	if(numOfPhilosophers > 500)
	{
	    printf("Sorry, please enter a value less than 501.\n");
	    type = DECIMAL;
	}
     }while(type != INTEGER);

    do{ // gets the number of meals from the user.      
        printf("\nEnter the number of meals (0 - 500): ");
        getInput(userInput, 255);
        type = CheckType(userInput);
        numOfMeals = atoi(userInput);
        if(type != INTEGER){
            printf("Sorry, only positive integer values are allowed.");
        }
	if(numOfMeals > 500)
	{
	    printf("Sorry, please enter a value less than 501.\n");
	    type = DECIMAL;
	}
    
   }while(type != INTEGER);

    // This array will be used to determine which chopsticks are available.
    Semaphore *chopstickArray[numOfPhilosophers];
    for(int i = 0; i < numOfPhilosophers; i++)
        chopstickArray[i] = new Semaphore("Chopstick", 1);

    s_chopsticks =  chopstickArray;
    // global pointer chopsticks points the the array so we can
    // look inside the array in other functions.


    S_Philosopher philosopherArray[numOfPhilosophers];
     // An array that will hold every philosopher.

    s_philosopherArrayPointer = philosopherArray;
    // global philosopherArrayPointer  points to the array so we 
    // can access methods to all philosophers in other functions. 


    // Forks a thread for each philosopher.
    for( int i = 0; i < numOfPhilosophers; i++)
    {
        Thread *t = new Thread("Philosopher");
        t->Fork(S_BeginDining, 0);
        currentThread->Yield();
    }
    sit.V();
}


#endif


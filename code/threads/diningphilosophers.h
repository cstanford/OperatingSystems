#ifndef DINING_PHILOS
#define DINING_PHILOS

#include "system.h"
#include "inputidentification.h"
#include "philosopher_globals.h"
#include "philosopher.h"

 // dummyParameters is only a placeholder to satisy the required parameters 
 // when forking a thread.
void BeginDining(int dummyParameter) 
{
    Philosopher phil(numOfPhilosophers, numOfMeals, chopsticks);
    philosopherArrayPointer[philosopherIdNumber] = phil;
    phil.setId(philosopherIdNumber);
    philosopherIdNumber++;
    phil.join();

    while(philosopherIdNumber <= numOfPhilosophers)
    {
        phil.busyWait(); // Causes thread to yield between 2 and 5 cycles.

        if(philosopherIdNumber == numOfPhilosophers) { 
            //  once all philosophers have entered the room, each philosopher
            //  will sit down.
            phil.sit();
            break;
        }

    }


    // Ensures that all philosophers have sat before continuting. 
    for (int i = 0; i < numOfPhilosophers; i++)
    {
        if(philosopherArrayPointer[i].hasSat() == false)
            phil.busyWait();

    }
    
    do // Continue eating until all meals are gone.
    {
	phil.beginEating();
	currentThread->Yield();
    }while(numOfMeals > 0);


    // Ensures that the philosophers stay seated until all are finished eating.     
    for(int i = 0; i < numOfPhilosophers; i++)
    {
	if(philosopherArrayPointer[i].isReadyToLeave() == false)
	    phil.wait();
    }

    // Once the first thread prints the statement, no others will be able to. 
    while(philosophersHaveLeft == false)
    {	
	philosophersHaveLeft = true;
	printf("All philosophers leave the table together.\n\n");
	
    }
    

}


void DiningPhilosophers(int dummyParameters)
 // dummyParameters is only a placeholder to satisy the required parameters 
 // when forking a thread.
{
    char userInput[256];
    input_type type;

    do{ // gets the number of philosophers from the user.
        printf("\nEnter the number of philosophers: ");
        getInput(userInput, 255);
        type = CheckType(userInput);
        numOfPhilosophers = atoi(userInput);
        if(type != INTEGER){
            printf("Sorry, only positive integer values are allowed.");
        }
     }while(type != INTEGER);

    do{ // gets the number of meals from the user.      
        printf("\nEnter the number of meals: ");
        getInput(userInput, 255);
        type = CheckType(userInput);
        numOfMeals = atoi(userInput);
        if(type != INTEGER){
            printf("Sorry, only positive integer values are allowed.");
        }
   }while(type != INTEGER);

    bool chopstickArray[numOfPhilosophers];
     // This array will be used to determine which chopsticks are available.
     // True indicates that the chopstick is available. False otherwise.


    // simply doing:  bool chopstickArray[numOfPhilosophers] = {true};
    // does not work becuase the array size is a variable.
    for(int i = 0; i < numOfPhilosophers; i++)
    {
        chopstickArray[i] = true; // initially, all chopsticks are available.
    }

    chopsticks =  chopstickArray;
     // global pointer chopsticks points the the array so we can
     // look inside the array in other functions.


    Philosopher philosopherArray[numOfPhilosophers];
     // An array that will hold every philosopher.

    philosopherArrayPointer = philosopherArray;
    // global philosopherArrayPointer  points to the array so we 
    // can access methods to all philosophers in other functions. 


    // Forks a thread for each philosopher.
    for( int i = 0; i < numOfPhilosophers; i++)
    {
        Thread *t = new Thread("Philosopher");
        t->Fork(BeginDining, 0);
        currentThread->Yield();
    }


}


#endif


#ifndef DINING_PHILOS
#define DINING_PHILOS

#include "system.h"
#include "inputidentification.h"
#include "philosopher.h"

int numOfPhilosophers = 0;  // Number of philosophers entered by the user.
int numOfMeals = 0;	    // Number of meals entered by the user.

int philosopherIdNumber = 0;
  // Will be incremented for each philosopher, and used for method 
  // setId(int philosopherIdNumber) in the philosopher class.
	    

bool *chopsticks; 
 // Will be pointed to an array  used to determine which chopsticks are 
 // available. True indicates that the chopstick is available. False otherwise.


Philosopher *philosopherArrayPointer;
 // Will point to an array of philosophers.


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
        phil.wait(); // Causes thread to yield between 2 and 5 cycles.

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
        if(philosopherArrayPointer[0].hasSat() == false)
            phil.wait();

    }

    phil.beginEating();

}


void DiningPhilosophers(int dummyParameter)
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
        t->Fork(BeginDining, philosopherIdNumber);
        currentThread->Yield();
    }

}


#endif


#ifndef P_GLOBS_
#define P_GLOBS_
#include "synch.h"
#include "philosopher.h"
#include "s_philosopher.h"

int numOfPhilosophers = 0;  // Number of philosophers entered by the user.
int numOfMeals = 0;	    // Number of meals entered by the user.

int philosopherIdNumber = 0;
  // Will be incremented for each philosopher, and used for method 
  // setId(int philosopherIdNumber) in the philosopher class.
	    

bool *chopsticks; 
Semaphore **s_chopsticks;
 // Will be pointed to an array  used to determine which chopsticks are 
 // available. True indicates that the chopstick is available. False otherwise.


Philosopher *philosopherArrayPointer;
S_Philosopher *s_philosopherArrayPointer;
 // Will point to an array of philosophers.

bool philosophersHaveLeft = false;
 // Used to prevent each thread from printing the statement:
 //  "All philosophers leave the table together."


#endif

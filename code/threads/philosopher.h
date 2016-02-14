#ifndef PHIL_H
#define PHIL_H

#include "system.h"


class Philosopher {

    public:
    Philosopher();
    Philosopher(int numOfPhilosophers, int &numOfMeals, bool chopsticksArray[], bool ifUsingSemaphores);
    ~Philosopher();

    int getId();
    void setId(int idNumber);
    void join();
     // Used when the philosopher has entered the room and is
     // waiting to sit.

    bool hasSat(); // Returns true if the philosopher has taken a seat.
    void sit();	   // Sets isSitting to true.
    void pickUpLeftChopstick();
    void pickUpRightChopstick();
    void putDownLeftChopstick();
    void putDownRightChopstick();
    void beginEating();
    void wait();    // Yields bewteen 2 and 5 cycles.
    void busyWait();    // Yields bewteen 2 and 5 cycles.
    void think();   // Same as wait but prints output.

    bool isReadyToLeave(); // Returns the value of ready to leave.

    private:
    bool useSemaphores;
    bool isSitting;
    int id;
    bool *chopsticks;
     // points to the array of chopsticks created in diningphilosophers.h

    int numberOfPhilosophers;
    int *numberOfMeals; 
     // points to the number of meals created in diningphilosophers.h
     // Note that numOfMeals is passed by reference. 
    
    bool hasBothChopsticks;
     // inidcates if the philosopher holds both chopsticks.

    bool readyToLeave;
     // True if the philosopher is waiting to leave the table.

};

Philosopher::Philosopher() {

}

Philosopher::Philosopher(int numOfPhilosophers, int &numOfMeals,
		         bool chopsticksArray[], bool ifUsingSemaphores) {
    id = 0;
    isSitting = false;
    useSemaphores = ifUsingSemaphores;
    numberOfPhilosophers = numOfPhilosophers;
    numberOfMeals = &numOfMeals;
    chopsticks = chopsticksArray;
    hasBothChopsticks = false;
    readyToLeave = false;
}

Philosopher::~Philosopher() {
}

int Philosopher::getId() {
    return id;
}

void Philosopher::setId(int idNumber) {

    id = idNumber;

}

void Philosopher::join() {

    printf("Philosopher %d has joined the room. \n", id);
    printf("Philosopher %d is waiting to sit. \n", id);

}

bool Philosopher::hasSat() {

    return isSitting;

}

void Philosopher::sit() {
    isSitting = true;
    printf("Philosopher %d has sat at the table. \n", id);

}

void Philosopher::pickUpLeftChopstick() {


    printf("Philosopher %d is trying to pick up chopstick %d. \n", id, id);
    
    // If the chopstick is available pick it up and mark as
    // unavailable. 
    if (chopsticks[id] == true){ 
        chopsticks[id] = false;
        printf("Philosopher %d has picked up chopstick %d. \n", id, id);
    }
    else { //wait for chopstick to become available.
	 while(chopsticks[id] == false && *numberOfMeals > 0)
	{
            wait();
        }
	// If number of meals are < 0 do not pick up chopstick.
        if ( *numberOfMeals <= 0){
            return;
        }

        chopsticks[id] = false;
        printf("Philosopher %d has picked up chopstick %d. \n", id, id);
    }
}


void Philosopher::pickUpRightChopstick() {
    
    // Index number of the philosophers right chopstick.
    int rightChopstickIndex = (id + 1) % numberOfPhilosophers; 

    printf("Philosopher %d is trying to pick up chopstick %d. \n", id, rightChopstickIndex);

    // If the chopstick is available pick it up and mark as
    // unavailable. 
    if (chopsticks[ rightChopstickIndex ] == true) 
    {
        chopsticks[ rightChopstickIndex ] = false;
	hasBothChopsticks = true;
	  // if the philospher has picked up the right chopstick, he is already holding the left. 


        printf("Philosopher %d has picked up chopstick %d. \n", id, rightChopstickIndex);
	return;
    }
    else
     {
	int waitCount = 0;	
        while(chopsticks[ rightChopstickIndex ] == false && *numberOfMeals > 0 ) 
	{
	     waitCount++;
	     wait();
	    if(waitCount >= 5)
	    {
		printf("Philosopher %d has aborted picking up chopstick %d to prevent deadlock after 5 cycles!\n", id, rightChopstickIndex);
		putDownLeftChopstick();
		return;
	    }
        }

	// If number of meals are < 0 do not pick up chopstick.
        if (*numberOfMeals <= 0)
	{
	    putDownLeftChopstick();
            return;
        }

        chopsticks[ rightChopstickIndex ] == false;
	hasBothChopsticks = true;
        printf("Philosopher %d has picked up chopstick %d. \n", id, rightChopstickIndex);
    }
}

void Philosopher::putDownLeftChopstick() {

    chopsticks[id] = true;
    hasBothChopsticks = false;
    printf("Philosopher %d has put down chopstick %d. \n", id, id);

}

void Philosopher::putDownRightChopstick() {

    chopsticks[ (id + 1) % numberOfPhilosophers ] = true;
    printf("Philosopher %d has put down chopstick %d. \n", id, (id + 1) % numberOfPhilosophers);

}

void Philosopher::beginEating()
{
    pickUpLeftChopstick();
    pickUpRightChopstick();
    
    if(!hasBothChopsticks)
    { // If the philospher was not able to pick up both chopsticks 
      // he should not continue eating.
	if( *numberOfMeals <= 0)
	{
	    readyToLeave = true;
	    printf("Philosopher %d is waiting to leave...\n", id);
	}
	return;
    }
    
    if(*numberOfMeals > 0)
    {
        *numberOfMeals = *numberOfMeals - 1; 
        printf("Philosopher %d has begun eating. %d meals remaining... \n", id, *numberOfMeals);
        busyWait();
        putDownLeftChopstick();
        putDownRightChopstick();
        think();
	
	if( *numberOfMeals <= 0)
	{
	    readyToLeave = true;
	    printf("Philosopher %d is waiting to leave...\n", id);
	}
    }
   else
   {
	readyToLeave = true;
	printf("Philosopher %d is waiting to leave...\n", id);
   }
    

}

void Philosopher::busyWait() {
    //Sometimes it is useful to simply yield, otherwise determine which to do based on task number
    //Used for thinking and eating
    int randInt = Random() % 4 + 2; // random number between 2 and 5.

    for(int i = 0; i < randInt; i++)
    {
        currentThread->Yield();
    }
}

void Philosopher::wait() {

    if(!useSemaphores){
        int randInt = Random() % 4 + 2; // random number between 2 and 5.

        for(int i = 0; i < randInt; i++)
        {
            currentThread->Yield();
        }
    }

}

void Philosopher::think() {

    printf("Philosopher %d is thinking... \n", id);
    busyWait();

}

bool Philosopher::isReadyToLeave() {

    return readyToLeave;
}



#endif


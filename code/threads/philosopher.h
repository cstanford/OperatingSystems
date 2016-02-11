#ifndef PHIL_H
#define PHIL-H

#include "system.h"

class Philosopher {

    public:
    Philosopher();
    Philosopher(int numOfPhilosophers, int &numOfMeals, bool chopsticksArray[]);
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
    void think();   // Same as wait but prints output.

    private:
    bool isSitting;
    int id;
    bool *chopsticks;
     // points to the array of chopsticks created in diningphilosophers.h

    int numberOfPhilosophers;
    int *numberOfMeals; 
     // points to the number of meals created in diningphilosophers.h
     // Note that numOfMeals is passed by reference. 


};

Philosopher::Philosopher() {

}

Philosopher::Philosopher(int numOfPhilosophers, int &numOfMeals, bool chopsticksArray[]) {
    id = 0;
    isSitting = false;
    numberOfPhilosophers = numOfPhilosophers;
    numberOfMeals = &numOfMeals;
    chopsticks = chopsticksArray;
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
        while(chopsticks[id] == false && *numberOfMeals > 0){
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
    if (chopsticks[ rightChopstickIndex ] == true){
        chopsticks[ rightChopstickIndex ] = false;
        printf("Philosopher %d has picked up chopstick %d. \n", id, rightChopstickIndex);
    }
    else {
        while(chopsticks[ rightChopstickIndex ] == false && *numberOfMeals > 0){
            wait();
        }

	// If number of meals are < 0 do not pick up chopstick.
        if (*numberOfMeals <= 0){
            return;
        }

        chopsticks[ rightChopstickIndex ] == false;
        printf("Philosopher %d has picked up chopstick %d. \n", id, rightChopstickIndex);
    }
}

void Philosopher::putDownLeftChopstick() {

    chopsticks[id] = true;
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

    if(*numberOfMeals > 0)
    {
        printf("Philosopher %d has begun eating. \n", id);
        *numberOfMeals = *numberOfMeals - 1;
        wait();
        putDownLeftChopstick();
        putDownRightChopstick();
        think();
    }

    if(*numberOfMeals > 0)
        beginEating();
    else
        return;

}


void Philosopher::wait() {

    int randInt = Random() % 6 + 2; // random number between 2 and 5.

    for(int i = 0; i < randInt; i++)
    {
        currentThread->Yield();
    }

}

void Philosopher::think() {

    printf("Philosopher %d is thinkking... \n", id);
    wait();

}


#endif


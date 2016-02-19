#ifndef PHIL_S_H
#define PHIL_S_H

#include "system.h"
#include "synch.h"


class S_Philosopher {

    public:
    S_Philosopher();
    S_Philosopher(int numOfPhilosophers, int &numOfMeals, Semaphore **chopsticksArray);
    ~S_Philosopher();

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
    bool isSitting;
    int id;
    Semaphore **chopsticks;
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

S_Philosopher::S_Philosopher() {

}

S_Philosopher::S_Philosopher(int numOfPhilosophers, int &numOfMeals, Semaphore **chopsticksArray) {
    id = 0;
    isSitting = false;
    numberOfPhilosophers = numOfPhilosophers;
    numberOfMeals = &numOfMeals;
    chopsticks = chopsticksArray;
    hasBothChopsticks = false;
    readyToLeave = false;
}

S_Philosopher::~S_Philosopher() {
}

int S_Philosopher::getId() {
    return id;
}

void S_Philosopher::setId(int idNumber) {

    id = idNumber;

}

void S_Philosopher::join() {

    printf("Philosopher %d has joined the room. \n", id);
    printf("Philosopher %d is waiting to sit. \n", id);

}

bool S_Philosopher::hasSat() {

    return isSitting;

}

void S_Philosopher::sit() {
    isSitting = true;
    printf("Philosopher %d has sat at the table. \n", id);

}

void S_Philosopher::pickUpLeftChopstick() { 
    printf("Philosopher %d is trying to pick up chopstick %d. \n", id, id);
    
    // If the chopstick is available pick it up and mark as
    // unavailable. 
    chopsticks[id]->P();
    printf("Philosopher %d has picked up chopstick %d. \n", id, id);
}


void S_Philosopher::pickUpRightChopstick() {
    // Index number of the philosophers right chopstick.
    int rightChopstickIndex = (id + 1) % numberOfPhilosophers; 

    printf("Philosopher %d is trying to pick up chopstick %d. \n", id, rightChopstickIndex);

    chopsticks[rightChopstickIndex]->P();
    hasBothChopsticks = true;
    printf("Philosopher %d has picked up chopstick %d. \n", id, rightChopstickIndex);
    
}

void S_Philosopher::putDownLeftChopstick() {
    chopsticks[id]->V();
    printf("Philosopher %d has put down chopstick %d. \n", id, id);

} 
void S_Philosopher::putDownRightChopstick() {
    int rightChopstickIndex = (id + 1) % numberOfPhilosophers; 

    chopsticks[rightChopstickIndex]->V();
    printf("Philosopher %d has put down chopstick %d. \n", id, rightChopstickIndex);
}

void S_Philosopher::beginEating()
{
    if(!readyToLeave){
        int rightChopstickIndex = (id + 1) % numberOfPhilosophers; 

        //One of the solutions to preventing deadlock is to have a resource heirarchy.. that is, 
        //if you are a philosopher, you must pick up your lower numbered chopstick first
        //Then you can pick up the other one
        if(rightChopstickIndex > id){
            pickUpLeftChopstick();
            pickUpRightChopstick();
        }else {
            pickUpRightChopstick();
            pickUpLeftChopstick();
        }
        
        if(*numberOfMeals > 0)
        {
            *numberOfMeals = *numberOfMeals - 1; 
            printf("Philosopher %d has begun eating. %d meals remaining... \n", id, *numberOfMeals);
            busyWait();
            putDownLeftChopstick();
            putDownRightChopstick();
            think();
        }	
        else{ 
            putDownLeftChopstick();
            putDownRightChopstick();
            readyToLeave = true;
        }
    }

}

void S_Philosopher::busyWait() {
    //Sometimes it is useful to simply yield, otherwise determine which to do based on task number
    //Used for thinking and eating
    int randInt = Random() % 3 + 2; // random number between 2 and 5.

    for(int i = 0; i < randInt; i++)
    {
        currentThread->Yield();
    }
}

void S_Philosopher::wait() {

}

void S_Philosopher::think() {

    printf("Philosopher %d is thinking... \n", id);
    busyWait();

}

bool S_Philosopher::isReadyToLeave() {

    return readyToLeave;
}



#endif


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
    bool hasSat();
    void sit();
    void pickUpLeftChopstick();
    void pickUpRightChopstick();
    void putDownLeftChopstick();
    void putDownRightChopstick();
    void beginEating();
    void think();
    void wait();

    private:
    bool isSitting;
    int id;
    bool *chopsticks;
    int numberOfPhilosophers;
    int *numberOfMeals;


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

    if (chopsticks[id] == true){
        chopsticks[id] = false;
        printf("Philosopher %d has picked up chopstick %d. \n", id, id);
    }
    else {
        while(chopsticks[id] == false && *numberOfMeals > 0){
            wait();
        }

        if ( *numberOfMeals <= 0){
            return;
        }

        chopsticks[id] = false;
        printf("Philosopher %d has picked up chopstick %d. \n", id, id);
    }
}


void Philosopher::pickUpRightChopstick() {

    printf("Philosopher %d is trying to pick up chopstick %d. \n", id, (id + 1) % numberOfPhilosophers);

    if (chopsticks[ (id + 1) % numberOfPhilosophers ] == true){
        chopsticks[ (id + 1) % numberOfPhilosophers ] = false;
        printf("Philosopher %d has picked up chopstick %d. \n", id, (id + 1) % numberOfPhilosophers);
    }
    else {
        while(chopsticks[ (id + 1) % numberOfPhilosophers ] == false && *numberOfMeals > 0){
            wait();
        }

        if (*numberOfMeals <= 0){
            return;
        }

        chopsticks[ (id + 1) % numberOfPhilosophers ] == false;
        printf("Philosopher %d has picked up chopstick %d. \n", id, (id + 1) % numberOfPhilosophers);
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
        wait();
        putDownLeftChopstick();
        putDownRightChopstick();
        *numberOfMeals = *numberOfMeals - 1;
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


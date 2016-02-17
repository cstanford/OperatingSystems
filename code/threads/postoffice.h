#ifndef POSOFF_BUSY
#define POSOFF_BUSY

#include "system.h"
#include "inputidentification.h"
#include "person.h"
#include "synch.h"

int p = 0;	// # of people
int s = 0;	// max # of messages in a person's inbox
int m = 0;	// # messages sent before end of simulation

int *mailbox; 
 // Points to a 2D array. If a value of the mailbox is 0, then there is nothing in that slot.
 //  First index will indicate whose mailbox it is; 2nd will indicate which mail slot

int *mailboxcapacity;
 // Points to an array that holds the current capacity of the person's mailbox.
 // Size should be 0 < size < s, where s is max # of messages in a person's inbox

//Semaphore *semaphore; //("postofficesemaphore", 1);

void EnterPostOffice(int personID) {
    Person pers(personID, s, m, mailbox, mailboxcapacity, p);

    // While there are still messages to send.
    while (m > 0) {
	for (int i = 0; i < p; i++)
		printf("capacity new thread %d\n", mailboxcapacity[i]);

	pers.EnterPostOffice();
	//semaphore.P();
	pers.CheckMail();
	//semaphore.V();

	//randomize a recipient for mail; if randomize yourself, keep randomizing until it's someone else
	int randPerson = Random() % p;
	while (randPerson == personID) {
	    randPerson = Random() % p;
	}

	printf("Person %d wants to send Person %d a message.\n", personID, randPerson);
	printf(" Person %d checks - Person %d has %d messages in their mailbox.\n", personID, randPerson, mailboxcapacity[randPerson]);

	// if mailbox isn't at capacity, leave a message and decrement remaining messages to send, and leave post office
	if (mailboxcapacity[randPerson] < s) {	
	    pers.LeaveMessage(randPerson);
	    m -= 1;
	    pers.LeavePostOffice();
	}
	// Otherwise, enter a loop where you try to leave a message
	// To solve deadlock: Yield if unsucessful for 3 times. After your third failure,
	//  check your mail to clear out your inbox and then 
	else {
		printf("  Person %d's mailbox is full. Person %d will try again later.\n", randPerson, personID);
		for (int i = 0; i < 3; i ++) {
		    if (mailboxcapacity[randPerson] < s) {
			pers.LeaveMessage(randPerson);
			m -= 1;
		    }
	    	    else {
			printf("    Person %d yields for %dth time because Person %d's full mailbox.\n", personID, (i+1), randPerson);
			currentThread->Yield();
		    }
		}
		pers.CheckMail();
		currentThread->Yield();
		pers.LeaveMessage(randPerson);
		m -= 1;
	}
    }
}

void PostOfficeBusy(int fakeParameter) {
    
    char userInput[256];
    input_type type;

    // Input starts
    // gets the number of people participating in simulation.
    do{       
	printf("\nEnter the number of people: ");
	getInput(userInput, 255);
	type = CheckType(userInput);
	p = atoi(userInput);
	if(type != INTEGER)
	    printf("Sorry, only positive integer values are allowed.");
    } while(type != INTEGER);

    // gets the number of messages a person's inbox can hold.
    do{       
	printf("\nEnter the number of max messages in a person's inbox: ");
	getInput(userInput, 255);
	type = CheckType(userInput);
	s = atoi(userInput);
	if(type != INTEGER)
	    printf("Sorry, only positive integer values are allowed.");
    } while(type != INTEGER);

    // gets the number of messages sent before the simulation ends.
    do{       
	printf("\nEnter the number of messages sent before simulation ends: ");
	getInput(userInput, 255);
	type = CheckType(userInput);
	m = atoi(userInput);
	if(type != INTEGER)
	    printf("Sorry, only positive integer values are allowed.");
    } while(type != INTEGER);
    // Input ends

//    Semaphore semaphoreArray[p];
    int mailboxArray[p * s];
    int mailboxcapacityArray[p];

    for (int i = 0; i < p; i++)	{
	mailboxcapacityArray[i] = 0;	// loop that initializes mailboxCapacity to 0, meaning it's considered empty
	for (int j = 0; j < s; j++)
	    mailboxArray[i * p + j] = 0;	// nested loop that initializes mailbox values to 0, which is considered empty.
    }

    mailbox = mailboxArray;
    mailboxcapacity = mailboxcapacityArray;

    // forks p threads to enter the post office
    for( int i = 0; i < p; i++)
    {
        Thread *t = new Thread("Person");
        t->Fork(EnterPostOffice, i);
        currentThread->Yield();
    }

}


#endif

#ifndef POSOFF_BUSY
#define POSOFF_BUSY

#include "system.h"
#include "inputidentification.h"
#include "person.h"
#include "synch.h"

int p = 0;	// # of people
int s = 0;	// max # of messages in a person's inbox
int m = 0;	// # messages sent before end of simulation

int unread = 0; // # of unread mail at a given time

int *mailbox; 
 // Points to an array that holds the contents of everyone in the simulation's mailbox. 
 // If a value of the mailbox is 0, then there is nothing in that slot.

Semaphore **s_mailbox;

int *mailboxcapacity;
 // Points to an array that holds the current capacity of the person's mailbox.
 // Size should be 0 < size < s, where s is max # of messages in a person's inbox

Semaphore **s_mailboxcapacity;

//Semaphore **mailsema;
//  Points to an array that holds semaphores associated with particular mailbox slots (semaphore for *mailbox)

void EnterPostOffice(int personID) {
    Person pers(personID, s, mailbox, mailboxcapacity, p, s_mailboxcapacity, s_mailbox);

    // While there are still messages to send.
    do {
	pers.EnterPostOffice();
	pers.CheckMail();

	//randomize a recipient for mail; if randomize yourself, keep randomizing until it's someone else
	int randPerson = Random() % p;
	while (randPerson == personID) {
	    randPerson = Random() % p;
	}

	printf("Person %d wants to send Person %d a message.\n", personID, randPerson);
	printf(" Person %d sees Person %d has %d messages in their mailbox.\n", personID, randPerson, mailboxcapacity[randPerson]);

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
    }while (m > 0);
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
	else if(p <= 1)
	    printf("Sorry, value must be greater than 1.");
    } while(type != INTEGER || p <= 1);

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

    Semaphore *mailboxcapacitySemaphore[p];
    for (int i = 0; i < p; i++)
	mailboxcapacitySemaphore[i] = new Semaphore("Capacity", 1);

    s_mailboxcapacity = mailboxcapacitySemaphore;
    

    Semaphore *mailboxSemaphore[p * s];
    for (int i = 0; i < p; i++)
	for (int j = 0; j < s; j++)
	    mailboxSemaphore[i * p + j] = new Semaphore("Mailbox", 1);
	

    s_mailbox = mailboxSemaphore;

    // Creates arrays for the mailbox, how much is in each mailbox, and semaphores for the mailbox capacity.
    int mailboxcapacityArray[p];
    int mailboxArray[p * s];


    // Intialize values of the arrays
    for (int i = 0; i < p; i++)	{
	mailboxcapacityArray[i] = 0;
	for (int j = 0; j < s; j++) {
	    mailboxArray[i * p + j] = 0;
	}
    }

    // Have the pointers point to the arrays
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

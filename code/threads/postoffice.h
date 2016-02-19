#ifndef POSOFF_BUSY
#define POSOFF_BUSY

#include "system.h"
#include "inputidentification.h"
#include "person.h"
#include "synch.h"

int numOfPeople;	// number of people that attend the post office
int maxMessages;	// number of messages a person's mailbox can hold at one time
int stamps;		// number of messages sent until simulation ends

//int personID = 0;

static int *mailbox; 		// Pointer to the mailboxes. making it static should make it be 1 that all the threads share.

bool postOfficeClosed = false;

Semaphore **s_mailbox;

Person **personArrayPointer;

int personID = 0;

void PostOffice(int dummyParameter) {
    Person *pers = new Person(maxMessages, mailbox, numOfPeople, s_mailbox, personArrayPointer);
    pers->setID(dummyParameter);
    personArrayPointer[dummyParameter] = pers;

    personID++;

    while(personID <= numOfPeople)
    {
        pers->Wait(); // Causes thread to yield between 2 and 5 cycles.
        if(personID == numOfPeople) { 
            //  once all people have been born, they can go to the postoffice
            break;
        }
    }

    do // Continue until all messages have been mailed
    {
	pers->Enter();
	pers->ReadMail();
	stamps -= pers->SendMail();
	pers->Wait();
    } while(stamps > 0);
//>>>>>>> Stashed changes

    printf("\n ***** The Post Office ran out of stamps! No more messages can be sent! *****\n");

    while(pers->getCurrentSize(dummyParameter) != 0){
	pers->ReadMail();
    }
// Ensures that the mail is all read
    for(int i = 0; i < numOfPeople; i++)
    {
	if(personArrayPointer[i]->readyToLeave == false)
	    pers->Wait();
    }
    
    while(postOfficeClosed == false)
    {	
	postOfficeClosed = true;
	printf("\n ***** All messages have been sent and read! *****\n");
	printf(" ***** All %d people have left the post office!  ***** \n", numOfPeople);
	printf(" ***** The Post Office closes for the day.  ***** ");
	
    }
}


void PostOfficeSimulation(int fakeParameter) {
    
    char userInput[256];
    input_type type;

    // gets the number of people participating in simulation.
    do{       
	printf("\nEnter the number of people: ");
	getInput(userInput, 255);
	type = CheckType(userInput);
	numOfPeople = atoi(userInput);
	if(type != INTEGER)
	    printf("Sorry, only positive integer values are allowed.");
	else if(numOfPeople <= 1)
	    printf("Sorry, value must be greater than 1.");
    } while(type != INTEGER || numOfPeople <= 1);

    // gets the number of messages a person's inbox can hold.
    do{       
	printf("\nEnter the number of max messages in a person's inbox: ");
	getInput(userInput, 255);
	type = CheckType(userInput);
	maxMessages = atoi(userInput);
	if(type != INTEGER)
	    printf("Sorry, only positive integer values are allowed.");
    } while(type != INTEGER);

    // gets the number of messages sent before the simulation ends.
    do{       
	printf("\nEnter the number of messages sent before simulation ends: ");
	getInput(userInput, 255);
	type = CheckType(userInput);
	stamps = atoi(userInput);
	if(type != INTEGER)
	    printf("Sorry, only positive integer values are allowed.");
    } while(type != INTEGER);

    s_mailbox = new Semaphore*[numOfPeople];
    for (int i = 0; i < numOfPeople; i++)
	s_mailbox[i] = new Semaphore("Mailbox", 1);

    mailbox = new int[numOfPeople * maxMessages];
    for (int i = 0; i < numOfPeople; i++) {
	for (int j = 0; i < maxMessages; i++) {
            mailbox[numOfPeople * i + j] = 0;
	}
    }

    personArrayPointer = new Person*[numOfPeople];

    // forks p threads to enter the post office
    for( int i = 0; i < numOfPeople; i++)
    {
        Thread *t = new Thread("Person");
        t->Fork(PostOffice, i);
        currentThread->Yield();
    }

}


#endif

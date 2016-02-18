#ifndef POSOFF_BUSY
#define POSOFF_BUSY

#include "system.h"
#include "inputidentification.h"
//#include "person.h"
#include "synch.h"

	// currently all the code for postoffice is in this file.

int numOfPeople;	// number of people that attend the post office
int maxMessages;	// number of messages a person's mailbox can hold at one time
int stamps;		// number of messages sent until simulation ends

//int personID = 0;

static int *mailbox; 		// Pointer to the mailboxes. making it static should make it be 1 that all the threads share.

bool postOfficeClosed = false;

Semaphore **s_mailbox; //not currently in use

struct Person {
	int id;
	int currentsize;
	bool readyToLeave;
};
Person *personArrayPointer;

int personID = 0;

void Wait() {
	int randNum = Random() % 4 + 2;
	//printf("Person %d will return in %d cycles.\n", pers.id, randNum);

	for (int i = 0; i < randNum; i++) {
	    currentThread->Yield();
	}
}

void PostOffice(int dummyParameter) {
    Person pers;
    pers.id = personID;
    pers.currentsize = 0;
    pers.readyToLeave = false;
    personArrayPointer[personID] = pers;
    personID++;


    while(personID <= numOfPeople)
    {
        Wait(); // Causes thread to yield between 2 and 5 cycles.
        if(personID == numOfPeople) { 
            //  once all people have been born, they can go to the postoffice
            break;
        }

    }

    do // Continue eating until all meals are gone.
    {
	//pers.Enter();
	//pers.ReadMail();
	//pers.SendMail();
	printf("Person %d has entered the Post Office.\n", pers.id);

// Read Mail
	printf("Person %d has %d unread messages.\n", pers.id, pers.currentsize);
    	    if (pers.currentsize == 0)
		printf("Person %d's mailbox is empty.\n", pers.id);
    	    else {
		while (pers.currentsize > 0) {
	 	    for (int i = 0; i < maxMessages; i++) {
	    		//s_mailbox[pers.id * numOfPeople + i]->P();
	    		if (mailbox[pers.id * numOfPeople + i] != 0) {
			    printf("Person %d reads [ message %d ].", pers.id, mailbox[pers.id * numOfPeople + i]);
			    mailbox[pers.id * numOfPeople + i] = 0;
			    pers.currentsize -= 1;
		//currentThread->Yield();
			//pers.Wait();
	    		}
			//s_mailbox[pers.id * numOfPeople + i]->V();
	  	    }
		}
    	    }
// End Read Mail



//Send Mail
	int randPerson = Random() % numOfPeople;
    	    while (randPerson == pers.id) {
	    randPerson = Random() % numOfPeople;
    	}

	int randMessage = Random() % 4 + 1;
    	int recipientSize = personArrayPointer[randPerson].currentsize;
    	printf("Person %d would like to send Person %d a message.\n", pers.id, randPerson);
    	printf(" Person %d checks Person %d's inbox. They have %d messages.\n", pers.id, randPerson, recipientSize);

	if (recipientSize == maxMessages) {
	  printf("Person %d's inbox is full. Person %d will wait.\n", randPerson, pers.id);
	  for (int i = 0; i < 4; i ++) {
		    if (recipientSize < maxMessages) {
			bool mailing = true;
	 		int j = 0;
	  		do {
	      		    if (mailbox[randPerson * numOfPeople + j] == 0) {
	          		printf("  Person %d has sent [ message %d ] to Person %d.\n", pers.id, randMessage, randPerson);
		  		mailbox[randPerson * numOfPeople + j] = randMessage;
	          		stamps -= 1;
		  		personArrayPointer[randPerson].currentsize += 1;
	          		mailing = false;
		  		break;
	      		    }
	      		j += 1;
          		} while (mailing);

			// make the above into a function for the love of god jesus christ
		    }
	    	    else if (i < 3) {
			printf("    Person %d yields for %dth time because Person %d's full mailbox.\n", pers.id, (i+1), randPerson);
			currentThread->Yield();
		    }
		    else if (i == 3) {
			printf("    **DEADLOCK PREVENTION** Person %d aborts sending mail to Person %d's full mailbox.\n", pers.id, randPerson);
			break;
		    }
	  }
	}
	else if (recipientSize < maxMessages) {
	  bool mailing = true;
	  int i = 0;
	  do {
	      if (mailbox[randPerson * numOfPeople + i] == 0) {
	          printf("  Person %d has sent [ message %d ] to Person %d.\n", pers.id, randMessage, randPerson);
		  mailbox[randPerson * numOfPeople + i] = randMessage;
	          stamps -= 1;
		  personArrayPointer[randPerson].currentsize += 1;
	          mailing = false;
		  break;
	      }
	      i += 1;
          } while (mailing);
	}
// End SendMail

	printf("Person %d has left the post office.\n", pers.id);
	Wait();
    } while(stamps > 0);

    printf("\n ***** The Post Office ran out of stamps! No more messages can be sent! *****\n");

// Ensures that the mail is all read
    for(int i = 0; i < numOfPeople; i++)
    {
	if(personArrayPointer[i].readyToLeave == false)
	    Wait();
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

    Semaphore *mailboxSemaphore[numOfPeople * maxMessages];
    for (int i = 0; i < numOfPeople; i++)
	for (int j = 0; j < maxMessages; j++)
	    mailboxSemaphore[i * numOfPeople + maxMessages] = new Semaphore("Mailbox", 1);

    s_mailbox = mailboxSemaphore;

    int mailboxArray[numOfPeople * maxMessages];
    for (int i = 0; i < numOfPeople; i++) {
	for (int j = 0; i < maxMessages; i++) {
            mailboxArray[numOfPeople * i + j] = 0;
	}
    }

    mailbox =  mailboxArray;
    Person personArray[numOfPeople];
    personArrayPointer = personArray;

    // forks p threads to enter the post office
    for( int i = 0; i < numOfPeople; i++)
    {
        Thread *t = new Thread("Person");
        t->Fork(PostOffice, i);
        currentThread->Yield();
    }

}


#endif

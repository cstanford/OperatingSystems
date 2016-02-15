#ifndef PERSON_H
#define PERSON_H

#include "system.h"

class Person {
    public:
	Person();
	Person(int numID, int maxMessages, int &numOfMessages, 
	       int *mailboxArray, int mailboxcapacityArray[], int numOfPeople);
	~Person();

	void EnterPostOffice();
	void CheckMail();
	void SendMail();
	void LeaveMessage(int recipient);
	bool CheckCapacity(int recipient);
	void LeavePostOffice();
	void Wait();

    private:
	int id;
	int maxsize;
	int *currentsize;
	int *remainingMessages;
	int *mailbox;
	int maxpeople;

	//To do: currentsize, mailbox, and remainingMessages are shared resources; implement semaphores where these are
	//To do: implement something to get rid of deadlock
	//To do: Fix 2D array issue
};

Person::Person() { 
}

Person::Person(int numID, int maxMessages, int &numOfMessages, 
	       int *mailboxArray, int mailboxcapacityArray[], int numOfPeople) {
    id = numID;
    maxsize = maxMessages;
    remainingMessages = &numOfMessages;
    mailbox = mailboxArray;
    currentsize = mailboxcapacityArray;
    maxpeople = numOfPeople;
}

Person::~Person() { 
}

void Person::EnterPostOffice() {
    printf("Person %d has entered the Post Office.", id);
    CheckMail();
}

void Person::CheckMail() {
    printf("Person %d checks their mailbox. They have %d messages.", id, currentsize[id]);
    if (currentsize[id] == 0)
	printf("  Person %d's mailbox is empty.", id);
    else {
	for (int i = 0; i < maxsize; i++) {
	    if (mailbox[id][i] != =) {
		printf("  Person %d reads [message %d]", id, mailbox[id][i]);
		mailbox[id][i] = 0;
	        currentsize[id] = currentsize[id] - 1;
	        currentThread->Yield();
	    }
	}
    } 

//note to self: need to change this.
// when they read first message, yield, someone sends them a message in first spot, 
//	and when they continue reading mail, they never know/realize that there's mail in the first spot
    SendMail();

}

void Person::SendMail() {
    int randPerson = Random() % maxpeople;
    if (randPerson == id)	// if randomize yourself, calls SendMail again to randomize a new 
	SendMail();
    else {
	if(*remainingMessages > 0) {
	    printf("Person %d wants to send Person %d a message.", id, randPerson);
	    if (currentsize[randPerson] == maxsize) {	// store function in a variable if doesn't do it right
		LeaveMessage(randPerson);
		LeavePostOffice();
	    }
	    else {
		printf("  Person %d's mailbox is full. Person %d will wait awhile.", randPerson, id);
		do {
		    currentThread->Yield();
	    	} while (currentsize[randPerson] == maxsize);
	    }
	}
    }
}

void Person::LeaveMessage(int recipient) {
    int randMessage = Random() % 6 + 1;
    currentsize[recipient] = currentsize[recipient] + 1;
    remainingMessages = remainingMessages - 1;
    for (int i = 0; i < maxsize; i++)
	if (mailbox[recipient][i] == 0) {
		mailbox[recipient][i] = randMessage;
		break;
	}
    printf("  Person %d left [ message %d ] for Person %d.", id, randMessage, recipient);    
}

bool Person::CheckCapacity(int recipient) {
    if (currentsize[recipient] == maxsize)
	return true;
    else return false;
}

void Person::LeavePostOffice() {
    printf("Person %d leaves Post Office.", id);
    Wait();
}

void Person::Wait() {
    int randNum = Random() % 6 + 2; // randomize num between 2-5
    printf("  Person %d will return in %d cycles.", id, randNum);

    for (int i = 0; i < randNum; i++)
	currentThread->Yield();
    EnterPostOffice();
}

#endif

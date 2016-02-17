#ifndef PERSON_H
#define PERSON_H

#include "system.h"
#include "synch.h"

class Person {
    public:
	Person();
	Person(int numID, int maxMessages, int &numOfMessages, 
	       int mailboxArray[], int mailboxcapacityArray[], int numOfPeople);
	~Person();

	void EnterPostOffice();
	void CheckMail();
	void SendMail();
	void LeaveMessage(int recipient);
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
};

Person::Person() { 
}

Person::Person(int numID, int maxMessages, int &numOfMessages, 
	       int mailboxArray[], int mailboxcapacityArray[], int numOfPeople) {
    id = numID;
    maxsize = maxMessages;
    remainingMessages = &numOfMessages;
    mailbox = mailboxArray;
    currentsize = mailboxcapacityArray;
    maxpeople = numOfPeople;
}

Person::~Person() { 
}

// Person enters post office and calls CheckMail function
void Person::EnterPostOffice() {
    printf("Person %d has entered the Post Office.\n", id);
    //CheckMail();
}

// Person checks mailbox. If they have no messages, they send mail. If they do, they first read their mail then send mail.
void Person::CheckMail() {
    printf("Person %d checks their mailbox. They have %d messages.\n", id, currentsize[id]);
    if (currentsize[id] == 0) {
	printf("  Person %d's mailbox is empty.\n", id);
}
    else {
	for (int i = 0; i < maxsize; i++) {
	    if (mailbox[id * maxpeople + i] != 0) {
		printf("  Person %d reads [message %d]\n", id, mailbox[id * maxpeople + i]);
		mailbox[id * maxpeople + i] = 0;
	        currentsize[id] = currentsize[id] - 1;
	        //currentThread->Yield();
	    }
	}
    } 
    //SendMail();

}

// Person sends mail to someone if there are remaining messages left to send & leaves post office. If recipient's mailbox is full, they'll wait to send mail.
/*void Person::SendMail() {
    int randPerson = Random() % maxpeople;
    if (randPerson == id)	// if randomize yourself, calls SendMail again to randomize a new 
	SendMail();
    else {
	if(*remainingMessages > 0) {
	    printf("Person %d wants to send Person %d a message.\n", id, randPerson);
	    if (currentsize[randPerson] < maxsize) {
		LeaveMessage(randPerson);
		LeavePostOffice();
	    }
	    else {
		printf("  Person %d's mailbox is full. Person %d will try again later.\n", randPerson, id);
		for (int i = 0; i < 3; i ++) {
		    if (currentsize[randPerson] < maxsize)
			LeaveMessage(randPerson);
	    	    else currentThread->Yield();
		CheckMail();
		}
	    }
	}
    }
}*/

// Person leaves random mail for recipient. Decrements remainingMessages to send.
void Person::LeaveMessage(int recipient) {
    int randMessage = Random() % 5 + 1;
    currentsize[recipient] = currentsize[recipient] + 1;
    //remainingMessages = remainingMessages - 1;
    for (int i = 0; i < maxsize; i++)
	if (mailbox[recipient * maxpeople + i] == 0) {
		mailbox[recipient * maxpeople + i] = randMessage;
		break;
	}
    printf("  Person %d left [ message %d ] for Person %d.\n", id, randMessage, recipient);    
}

// Leave officeand call Wait()
void Person::LeavePostOffice() {
    printf("   Person %d leaves Post Office.\n", id);
    Wait();
}

// Wait by yielding
void Person::Wait() {
    int randNum = Random() % 4 + 2; // randomize num between 2-5
    printf("    Person %d will return in %d cycles.\n", id, randNum);

    for (int i = 0; i < randNum; i++)
	currentThread->Yield();
    //EnterPostOffice();
}

#endif

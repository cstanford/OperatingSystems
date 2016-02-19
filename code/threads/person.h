#ifndef PERSON_H
#define PERSON_H

#include "system.h"
#include "synch.h"

class Person {
    public:
	Person();
	Person(int numID, int maxMessages, int mailboxArray[], 
	       int mailboxcapacityArray[], int numOfPeople, 
	       Semaphore **mailcapsema, Semaphore **mailboxsema);
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

	int *mailbox;
	int maxpeople;

	Semaphore **s_mailcap;
	Semaphore **s_mailbox;

	//To do: currentsize, mailbox are shared resources; implement semaphores where these are
	//To do: implement something to get rid of deadlock
};

Person::Person() { 
}

Person::Person(int numID, int maxMessages, int mailboxArray[], 
	       int mailboxcapacityArray[], int numOfPeople, 
	       Semaphore **mailcapsema, Semaphore **mailboxsema) {
    id = numID;
    maxsize = maxMessages;
    mailbox = mailboxArray;
    currentsize = mailboxcapacityArray;
    maxpeople = numOfPeople;
    
    //s_mailcap = mailcapsema;
    s_mailbox = mailboxsema;
}

Person::~Person() { 
}

// Person enters post office and calls CheckMail function
void Person::EnterPostOffice() {
    printf("Person %d has entered the Post Office.\n", id);
}

// Person checks mailbox. If they have no messages, they send mail. If they do, they first read their mail then send mail.
void Person::CheckMail() {
    //s_mailcap[id]->P();
    printf("Person %d checks their mailbox. They have %d messages.\n", id, currentsize[id]);
    if (currentsize[id] == 0) {
	printf("  Person %d's mailbox is empty.\n", id);
}
    else {
	for (int i = 0; i < maxsize; i++) {
	    s_mailbox[id * maxpeople + i]->P();
	    if (mailbox[id * maxpeople + i] != 0) {
		printf("  Person %d reads [message %d]\n", id, mailbox[id * maxpeople + i]);
		mailbox[id * maxpeople + i] = 0;
		currentsize[id] = currentsize[id] - 1;
	    }
	    s_mailbox[id * maxpeople + i]->V();
	}
    } 
    //s_mailcap[id]->V();
}

// Person leaves random mail for recipient.
void Person::LeaveMessage(int recipient) {
    //s_mailcap[recipient]->P();

    int randMessage = Random() % 5 + 1;
    currentsize[recipient] = currentsize[recipient] + 1;
    for (int i = 0; i < maxsize; i++) {
        s_mailbox[recipient * maxpeople + i]->P();
	if (mailbox[recipient * maxpeople + i] == 0) {
		mailbox[recipient * maxpeople + i] = randMessage;
		break;
	}
        s_mailbox[recipient * maxpeople + i]->V();
    }
    printf("  Person %d left [ message %d ] for Person %d.\n", id, randMessage, recipient);    
    //s_mailcap[recipient]->V();
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
}

#endif

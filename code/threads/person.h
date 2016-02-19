#ifndef PERSON_H
#define PERSON_H

#include "system.h"
#include "synch.h"
class Person;
Person **people = new Person*[101];
class Person {
    public:
	Person();
	Person(int maxMessages, int mailboxArray[], int numOfPeople, 
		Semaphore **mailboxsema, Person **personPointerArray);
	~Person();

	void Enter();
	void ReadMail();
	bool SendMail();
	void LeavePostOffice();
	void Wait();

	bool readyToLeave;
	int getCurrentSize(int index);
	bool isReadyToLeave();
	void setID(int personID);
	void receiveLetter(int pat);

    private:
	int id;
	int maxsize;
	int currentsize;

	int *mailbox;
	int maxpeople;

	//Person **people;

	Semaphore **s_mailbox;

	//To do: currentsize, mailbox are shared resources; implement semaphores where these are
};

Person::Person() { 

}

Person::Person(int maxMessages, int mailboxArray[], int numOfPeople, 
	Semaphore **mailboxsema, Person **personArrayPointer) {
    id = 0;
    maxsize = maxMessages;
    mailbox = mailboxArray;
    currentsize = 0;
    maxpeople = numOfPeople;
    
    s_mailbox = mailboxsema;
    readyToLeave = false;
    //people = personArrayPointer;
}

Person::~Person() { 
}

// Person enters post office and calls CheckMail function
void Person::Enter() {
    printf("Person %d has entered the Post Office.\n", id);
}

void Person::receiveLetter(int pat){
    currentsize++;
    mailbox[id*maxpeople + currentsize-1] = pat;
}

// Person checks mailbox. If they have no messages, they send mail. If they do, they first read their mail then send mail.
void Person::ReadMail() {
	s_mailbox[id]->P();
	printf("Person %d has %d unread messages.\n", id, currentsize);
    	if (currentsize == 0) {
	    printf("Person %d's mailbox is empty.\n", id);
	}
    	else {
	    while (currentsize > 0) {
		printf("Person %d reads [ pattern %d ].\n", id, mailbox[id * maxpeople + currentsize - 1]);
		mailbox[id * maxpeople + currentsize - 1] = 0;
		currentsize -= 1;
		currentThread->Yield();
	    }
	}
	s_mailbox[id]->V();
}

// Person leaves random mail for recipient.
bool Person::SendMail() {

	bool sent = false;

	// Randomizes person until you get someone that isn't you
	int randPerson = Random() % maxpeople;
    	while (randPerson == id) {
	    randPerson = Random() % maxpeople;
    	}

	s_mailbox[randPerson]->P();

	int randMessage = Random() % 4 + 1;
    	int recipientSize = getCurrentSize(randPerson);
    	printf("Person %d would like to send Person %d a message.\n", id, randPerson);
    	printf(" Person %d checks Person %d's inbox. They have %d messages.\n", id, randPerson, recipientSize);

	// If Mailbox for recipient is full
	if (recipientSize == maxsize) {
	printf("Person %d's inbox is full. Person %d will wait.\n", randPerson, id);
	    for (int i = 0; i < 4; i ++) {
		recipientSize = getCurrentSize(randPerson);
		if (recipientSize < maxsize) {		// Yay! Some mail was read. mail something.
		    printf("  Person %d has sent [ pattern %d ] to Person %d.\n", id, randMessage, randPerson);		    
		    people[randPerson]->receiveLetter(randMessage);

		    sent = true;
		    break;
		}
	    	else if (i < 3) {
		    printf("    Person %d yields for %dth time because Person %d's full mailbox.\n", id, (i+1), randPerson);
			s_mailbox[randPerson]->V();
			currentThread->Yield();
			s_mailbox[randPerson]->P();
		}
		else if (i == 3) {
		    printf("    **DEADLOCK PREVENTION** Person %d aborts sending mail to Person %d's full mailbox.\n", id, randPerson);
		    break;
		}
	    }
	}
	// If recipient's mailbox isn't full
	else if (recipientSize < maxsize) {
		printf("  Person %d has sent [ pattern %d ] to Person %d.\n", id, randMessage, randPerson);		
	        people[randPerson]->receiveLetter(randMessage);
		sent = true;
	}
	s_mailbox[randPerson]->V();

	return sent;

}

// Leave office
void Person::LeavePostOffice() {
    printf("   Person %d leaves Post Office.\n", id);
}

// Wait by yielding
void Person::Wait() {
    int randNum = Random() % 4 + 2; // randomize num between 2-5
    printf("    Person %d will return in %d cycles.\n", id, randNum);

    for (int i = 0; i < randNum; i++)
	currentThread->Yield();
}

int Person::getCurrentSize(int index) {
	return people[index]->currentsize;
}

void Person::setID(int personID) {
	id = personID;
	people[id] = this;
}

bool Person::isReadyToLeave() {
	return readyToLeave;
}

#endif

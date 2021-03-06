#include "pcbList.h"

pcbList::pcbList(){
    next = NULL;
    data = NULL;
    id = NULL;
}

void pcbList::append(Thread* elem){
    pcbList* temp = this;
    if(data== NULL){
	data = elem;
	id = elem->getThisThreadID();
	return;
    } 

    for(; temp->next != NULL; temp = temp->next); //Goes to the end of the list
    //Now L points to the end of the list;
    
    temp->next = new pcbList;
    temp = temp->next;//temp->next is where we will store the new element
    temp->data = elem;
    temp->id = elem->getThisThreadID();
}

void pcbList::remove(Thread* elem){
    pcbList* temp = this;
    int id = elem->getThisThreadID();
    for(; temp->next != NULL; temp = temp->next){
	if(temp->next->id == id){ //Found the right thread to remove
	    pcbList* rmv = temp->next;
	    temp->next = rmv->next;
	    delete rmv; //remove it then
	    break;	//break out
	}
    }
    //otherwise we haven't found it after traversing whole list
    
}
bool pcbList::isValidID(int id){
    pcbList* temp = this;
    for(; temp != NULL; temp = temp->next){
	if(temp->id == id){
	    return true;
	}
    }
    return false; // Reached the end of the list and haven't found id
}


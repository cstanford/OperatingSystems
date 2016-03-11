#ifndef PCB_LIST_H
#define PCB_LIST_H
#include "thread.h"

class Thread;

struct pcbList
{ 
    pcbList();
    pcbList* next;
    Thread* data;
    int id;
    void append(Thread* elem);

    void remove(Thread* elem);
    bool isValidID(int id);
};

#endif

#include "system.h"
#include "inputidentification.h"

// Edited by Connor Stanford and Joseph Fontenot 

/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//                                                                                                       // 
// PLACE THIS SNIPPIT OF CODE in THREADTEST.CC to fork a thread to ShoutItOut:                           // 
//                                                                                                       // 
//   #include "shout.h"                                                                                  // 
//                                                                                                       // 
//   Thread *shout = new Thread("ShoutItOutThread");                                                     // 
//   shout->Fork(ShoutItOut, 0); // 0 is a fake parameter to satisfy the requirements to fork a thread.  // 
//                                                                                                       // 
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 


int numOfShouts = 0; // Global variable indcating the user-selected number of shouts 


void ExecuteShouts(int threadNumber) // Prints random "Shouts" to console. 
{

    char shout0[256] = "Do or do not, there is no try.";
    char shout1[256] = "I find your lack of faith disturbing.";
    char shout2[256] = "I've got a bad feeling about this.";
    char shout3[256] = "Never tell me the odds.";
    char shout4[256] = "You underestimate the power of the dark side.";
    char shout5[256] = "It's A trap!";


    int randInt;
    int timesShouted = 0;

     while(timesShouted < numOfShouts){
        switch(randInt = Random() % 6)
        {
          case 0:
                  printf("Shouter %d: %s\n", threadNumber,  shout0);
                  break;
          case 1:
                  printf("Shouter %d: %s\n", threadNumber,  shout1);
                  break;
          case 2:
                  printf("Shouter %d: %s\n", threadNumber,  shout2);
                  break;
          case 3:
                  printf("Shouter %d: %s\n", threadNumber, shout3);
                  break;
          case 4:
                  printf("Shouter %d: %s\n", threadNumber, shout4);
                  break;
          case 5:
                  printf("Shouter %d: %s\n", threadNumber, shout5);
                  break;
        }
        timesShouted++;
        for(int i = 0; i < 5; i++) // guarantees that other threads have a chance on CPU. 
           currentThread->Yield();
     }
}

void ShoutItOut(int fakeParameter) // fakeParameter is only a placeholder to satisfy the required parameters when forking a thread 
{

    int numOfThreads = 0; // Number of threads the user selects to run; 
    char userInput[256];
    input_type type;

    do{
        printf("\nEnter the number of threads: ");
        getInput(userInput, 255);
        type = CheckType(userInput);
        numOfThreads = atoi(userInput);
        if(type != INTEGER) {
            printf("Sorry, only positive integer values are allowed.");
        }
    }while(type != INTEGER);

    do{
        printf("\nEnter the number of shouts: ");
        getInput(userInput, 255);
        type = CheckType(userInput);
        numOfShouts = atoi(userInput);
        if(type != INTEGER) {
            printf("Sorry, only positive integer values are allowed.");
        }
    }while(type != INTEGER);

    printf("\n");

    int threadNumber = 1; // Use to give a unique ID to each thread. It is incremented each time a thread is forked. 
    for( int i = 0; i < numOfThreads; i++)
    {
      Thread *t = new Thread("Shouter");
      t->Fork(ExecuteShouts, threadNumber);
      threadNumber++;
      currentThread->Yield();
    }


}


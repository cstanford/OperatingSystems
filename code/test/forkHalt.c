#include "syscall.h"
int
main()
{
    //Write("\nForkHalt\n", 10, 1);


    int i = 0;
   // Exec("../test/Halt"); //broken

    //while(i < 1){
	//Exec("../test/halt");// <-- DOES NOT WORK
	Exec("../test/forkHalt");// <-- WORKS ONLY IF EXIT(0) BELOW!
//	Exit(0);
   // }

}

#include "syscall.h"
int
main()
{
    //Write("\nForkHalt\n", 10, 1);
    int i = 0;
    for(i = 0; i < 10; i++){
	Join(Exec("../test/sort"));
    }
    Exit(0);

}

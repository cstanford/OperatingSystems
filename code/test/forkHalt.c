#include "syscall.h"
int
main()
{
    //Write("\nForkHalt\n", 10, 1);
    int i = 0;
    Exec("../test/Halt");
    Exit(0);

}

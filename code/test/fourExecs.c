#include "syscall.h"
int
main()
{
    //Exec Sort test multiple times.
    Exec("../test/sort");
    Exec("../test/sort");
    Exec("../test/sort");
    Exec("../test/sort");
    //Exec("../test/halt");
    //Exec("../test/halt");
    //Exec("../test/halt");

    Exit(0);

}

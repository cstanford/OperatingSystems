#include "syscall.h"
int
main()
{
    //Exec Sort test multiple times.
    Exec("../test/sort");
    Exec("../test/sort");
    Exec("../test/sort");
    Exec("../test/sort");

    Exit(0);

}

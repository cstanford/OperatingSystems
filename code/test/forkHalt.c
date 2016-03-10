#include "syscall.h"
int
main()
{
    Write("KRISTA YO\n", 10, 1);
    Join(Exec("../test/halt"));
    Write("JUST WOKE\n", 10, 1);
    Join(Exec("../test/halt"));
    Write("JUST WOKE\n", 10, 1);
    Exit(0);
}

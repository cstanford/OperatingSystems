#include "syscall.h"
int
main()
{
    Write("KRISTA YO\n", 10, 1);
    Exec("../test/matmult");
}

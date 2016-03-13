#include "syscall.h"
int
main()
{
    //Join on sort test multiple times.
    int i = 0;
    int id = Exec("../test/sort");
    for(i = 0; i < 10; i++){
	Join(id);
    }
    Exit(0);

}

#include "syscall.h"
int
main()
{
    //Join on sort test multiple times.
    int i = 0;
    for(i = 0; i < 10; i++){
	int id = Exec("../test/sort"); 
	Join(id);
	
    }
    Exit(0);

}

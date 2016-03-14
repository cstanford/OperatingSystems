#include "syscall.h"

int main()
{
    int i = 0;
    Exec("../test/fib");
    Join(Exec("../test/fib"));
    
    i = Exec("../test/fib");
    Join(i);
    Yield();
    Fork(i);  //testing unprogrammed sys call
    Join(999);
    Exec("../test/fib");
    

}

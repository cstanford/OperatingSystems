#include "syscall.h"
/*
int fibonacci(int n)
{
    if (n == 0 || n == 1)
	return n;
    else
	return fibonacci(n - 1) + fibonacci(n - 2);
}

*/

int
main()
{
    //int i = 10000;
    //int answer = fibonacci(i); 
    const int size = 1000;
    int i;
    int sum;
    sum = 0;
    for(i = 0; i < size; i++){
        sum += i;
    }
    Exit(sum);

}


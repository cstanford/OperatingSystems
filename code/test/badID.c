#include "syscall.h"

int main()
{
    int i = 0; 
    int id = 500;
    int id2 = Exec("../test/sort");
    Join(id);
    Join(id2);
    //Exec("../test/MindGeek");
    Exec("../test/badID.coff");
    for(i = 0; i < 10; i++){
	id = Exec("../test/sort"); 
	Join(id);
    }
}

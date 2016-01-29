#include "system.h"
#include "string.h"

//Code edited by Joseph Fontenot and Connor Stanford


bool isInteger(char* input){
    if(*input == '\0') return false;
    for(; *input != '\0'; input++){
        if(!(*input >= '0' && *input <= '9'))
            return false;
    }
    return true;
}
bool isNegativeInteger(char* input){
    if(*input == '-'){
        bool isInt = isInteger(++input);
        return isInt;
    }
    return false;
}
bool isDecimal(char* input){
    char *temp = input;
    int periodIndex = 0;
    for(int i = 0; *temp != '\0'; temp++, i++){
        if(*temp == '.')
            periodIndex = i;
    }

    //Can't be at 0, otherwise string
    if(periodIndex == 0) return false;

    char left[256];
    char right[256];

    int i;

    for(i = 0; i < periodIndex; i++)
        left[i] = *(input+i);
    left[i] = '\0';

    for(i = periodIndex+1; i < strlen(input); i++)
        right[i-periodIndex-1] = *(input+i);
    right[i-periodIndex-1] = '\0';
    return (isInteger(left) && isInteger(right));
}

bool isNegativeDecimal(char *input)
{
    if(*input == '-'){
        bool isNegDec = isDecimal(++input);
        return isNegDec;
    }
    return false;
}

bool isCharString(char *input)
{
    bool isInt = isInteger(input);
    bool isNegInt = isNegativeInteger(input);
    bool isDec = isDecimal(input);
    bool isNegDec = isNegativeDecimal(input);

    if ( isInt || isNegInt || isDec ||isNegDec)
    {
        return false;
    }
    return true;

}


void InputIdentification(int dummyParameter) // dummyParameter only here to satisfy forked 
{                                            // thread parameters.
    char input[256];


    printf("Please enter a value less than 256 characters: ");
    scanf("%s", input);

    bool isDec = isDecimal(input);
    bool isNegDec = isNegativeDecimal(input);
    bool isInt = isInteger(input);
    bool isNegInt = isNegativeInteger(input);
    bool isCharStr = isCharString(input);

    if (isCharStr)
        printf("Character String\n");
    else if (isNegDec)
        printf("Negative Decimal\n");
    else if (isNegInt)
        printf("Negative Integer\n");
    else if (isDec)
        printf("Decimal\n");
    else
        printf("Integer\n");


  /****** Used for testing ************  
    printf("NegInt: %d\n", isNegInt);
    printf("Int:    %d\n", isInt);
    printf("NegDec: %d\n", isNegDec);
    printf("Dec:    %d\n", isDec);
    printf("String: %d\n", isCharStr);
  *************************************/



}


#ifndef INPUT_IDENT_H
#define INPUT_IDENT_H
#include "system.h"
#include "string.h"

//Code edited by Joseph Fontenot and Connor Stanford
enum input_type {
    INTEGER,
    NEG_INTEGER,
    DECIMAL,
    NEG_DECIMAL,
    CHAR_STRING
};

void getInput(char* store, int maxBuff);
bool isInteger(char* input);
bool isNegativeInteger(char* input);
bool isDecimal(char* input);
bool isNegativeDecimal(char *input);
void InputIdentification(int dummyParameter);

input_type CheckType(char *input);

//Code edited by Joseph Fontenot and Connor Stanford
void getInput(char* store, int maxBuff){
    fgets(store, maxBuff, stdin);
    //fgets considers \n an endline but also stores is
    //So simply strip it off to get the string and keep
    //the null terminator
    if(*(store+strlen(store) -1) == '\n')
        *(store+strlen(store) - 1) = '\0';
}

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
    unsigned int periodIndex = 0;
    for(int i = 0; *temp != '\0'; temp++, i++){
        if(*temp == '.')
            periodIndex = i;
    }

    //Can't be at 0, otherwise string
    if(periodIndex == 0) return false;

    char left[256];
    char right[256];

    unsigned int i;

    for(i = 0; i < periodIndex; i++)
        left[i] = *(input+i);
    left[i] = '\0';

    for(i = periodIndex+1; i < strlen(input); i++)
        right[i-periodIndex-1] = *(input+i);
    right[i-periodIndex-1] = '\0';
    return (isInteger(left) && isInteger(right));
}

bool isNegativeDecimal(char *input){
    if(*input == '-'){
        bool isNegDec = isDecimal(++input);
        return isNegDec;
    }
    return false;
}

//enum input types defined in header
input_type CheckType(char *input) {
    bool isInt = isInteger(input);
    bool isNegInt = isNegativeInteger(input);
    bool isDec = isDecimal(input);
    bool isNegDec = isNegativeDecimal(input);
    bool isCharString = !(isInt || isNegInt || isDec || isNegDec);

    if(isInt) return INTEGER;
    else if(isNegInt) return NEG_INTEGER;
    else if(isDec) return DECIMAL;
    else if(isNegDec) return NEG_DECIMAL;
    else if(isCharString) return CHAR_STRING;
}

void PrintType(input_type t){
    switch(t){
    case INTEGER:
        printf("Integer\n");
        break;
    case NEG_INTEGER:
        printf("Negative Integer\n");
        break;
    case DECIMAL:
        printf("Decimal\n");
        break;
    case NEG_DECIMAL:
        printf("Negative Decimal\n");
        break;
    case CHAR_STRING:
        printf("Character String\n");
        break;
    };
}

// dummyParameter only here to satisfy forked 
// thread parameters.
void InputIdentification(int dummyParameter){
    char input[256];

    printf("Please enter a value less than 256 characters: ");
    getInput(input, 256);

    PrintType(CheckType(input));

  /****** Used for testing ************  
    printf("NegInt: %d\n", isNegInt);
    printf("Int:    %d\n", isInt);
    printf("NegDec: %d\n", isNegDec);
    printf("Dec:    %d\n", isDec);
    printf("String: %d\n", isCharStr);
  *************************************/
}

#endif

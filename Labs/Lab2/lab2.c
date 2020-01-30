#include<stdlib.h>
#include<stdio.h>

int strLen(const char* str){
    int i = 0;
   
    while(str[i] != 0){
        i++;
    }

    return i;
}

char* stringSort(const char* input){
    int size = strLen(input);
    char* output = malloc(size * sizeof(char));

    int letterCount = 0;
    int numCount = 0;

    for(int i = 0; i < size; i++){
        if((input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= 'a' && input[i] <= 'z')){ 
            letterCount ++;
        }
        if(input[i] >= '0' && input[i] <= '9'){
            numCount++;
        }
    }

    int letterIndex = 0;
    int numIndex = letterIndex + letterCount;
    int otherIndex = numIndex + numCount;

    for(int i = 0; i < size; i++){
        if((input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= 'a' && input[i] <= 'z')){
            output[letterIndex] = input[i];
            letterIndex ++;
        }else if(input[i] >= '0' && input[i] <= '9'){
            output[numIndex] = input[i];
            numIndex ++;
        }else{
            output[otherIndex] = input[i];
            otherIndex ++;
        }
    }

    return output;
}

char** threeArrayStringSort(char* input){
    int size = strLen(input);

    int letterCount = 0;
    int numCount = 0;
    int otherCount = 0;

    for(int i = 0; i < size; i++){
        if((input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= 'a' && input[i] <= 'z')){ 
            letterCount ++;
        }else if(input[i] >= '0' && input[i] <= '9'){
            numCount++;
        }else{
            otherCount++;
        }
    }

    char** output = calloc(3, sizeof(char*));

    output[0] = calloc(letterCount, sizeof(char));
    output[1] = calloc(numCount, sizeof(char));
    output[2] = calloc(otherCount, sizeof(char));

    int letterIndex = 0;
    int numIndex = 0;
    int otherIndex = 0;

    for(int i = 0; i < size; i++){
        if((input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= 'a' && input[i] <= 'z')){
            output[0][letterIndex] = input[i];
            letterIndex++;
        }else if(input[i] >= '0' && input[i] <= '9'){
            output[1][numIndex] = input[i];
            numIndex++;
        }else{
            output[2][otherIndex] = input[i];
            otherIndex++;
        }
    }

    return output;
}



int main(int argc, char* argv[]){
    for(int i = 1; i < argc; i++){
        char* output = stringSort(argv[i]);
        printf("%s\n", output);
        free(output);
    }

    for(int i = 1; i < argc; i++){
        char** output = threeArrayStringSort(argv[i]);
        printf("Letters: %s\nNumbers: %s\nOthers: %s\n", output[0], output[1], output[2]);
        free(output);
    }
}
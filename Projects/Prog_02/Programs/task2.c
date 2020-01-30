#include<stdlib.h>
#include<stdio.h>
#include<string.h>

/**
 * Function that, given a c-string and a char, loops through
 * the c-string until it finds either the character or the 
 * null terminator, returning 1 if the character is found, and
 * 0 if the null terminator is hit.
*/
int inString(const char* string, const char item){
    int len = strlen(string);

    for(int i = 0; i < len; i++){
        if(string[i] == item) return 1;
    }

    return 0;
}

/**
 * Gets the length of the given input using strlen,
 * then allocates an output using calloc of the same
 * size. It then loops through the input string, 
 * adding each character to the output string if it
 * isn't there, and ignoring it if it's not. It then
 * returns a pointer to the output string.
 */
char* removeDuplicates(const char* input){
    int inputLen = strlen(input);
    char* output = calloc(inputLen, sizeof(char));
    int outputIndex = 0;
    for(int i = 0; i < inputLen; i++){
        if(inString(output, input[i]) == 0){
            output[outputIndex] = input[i];
            outputIndex ++;
        }
    }

    return output;
}

/**
 * Comparator function for qsort(). Simply 
 * compares the values of the two given 
 * pointers, and if the first is lesser, return 
 * -1, if they're equal, return 0, and if the first
 * is greater, return 1.
 */
int comparison(const void* a, const void* b){
    if ( *(char*)a < *(char*)b) return -1;
    if ( *(char*)a == *(char*)b) return 0;
    if ( *(char*)a > *(char*)b) return 1;
}

/**
 * Given an input string, output a char** of 
 * that string sorted into 3 categories.
 */
char** threeArrayStringSort(char* input){
    /**
     * Get the size of the input, and declare
     * storage for the number of letters
     * the number of nums, and the number
     * of other items
     */
    int size = strlen(input);

    int letterCount = 0;
    int numCount = 0;
    int otherCount = 0;

    //At this stage, we'll be looping through the array
    //once, to count the number of letters and numbers 
    for(int i = 0; i < size; i++){
        if((input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= 'a' && input[i] <= 'z')){ 
            letterCount ++;
        }else if(input[i] >= '0' && input[i] <= '9'){
            numCount++;
        }else{
            otherCount++;
        }
    }

    //Allocating the output array, then allocating size for each
    //of the char*s in the output. We're also at this point going to
    //set the indices for each of the types
    char** output = calloc(3, sizeof(char*));

    output[0] = calloc(letterCount, sizeof(char));
    output[1] = calloc(numCount, sizeof(char));
    output[2] = calloc(otherCount, sizeof(char));

    int letterIndex = 0;
    int numIndex = 0;
    int otherIndex = 0;

    /**On our second loop through the input, we're actually assigning the values 
     * to their appropriate char*s, and then iterating the appropriate index.
     */
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

/**
 * Used to clear stdin, just uses getchar 
 * until it hits a newline character. getchar
 * removes each item from the stream, so it will
 * remove newlines, preventing infinite loops.
 */
void clearInputBuffer(){
    while(getchar() != '\n');
}

int main(int argc, char* argv[]){
    //Flag to see if we should exit, and the
    //char* to hold the input from the user
    int exit = 0;
    char input[101];

    //Input loop. Ask the user for input, then
    //wait. If it doesn't compare equal to EXIT,
    //then the user isn't ready to exit (or has
    //failed to do so properly), and we allocate
    //an array for the output, run that through
    //the removeDuplicates() function, and then
    //call the string sort function on it. We
    //then print it out per the requirements, 
    //free the memory allocated for output, 
    //and then clear the input buffer so we can
    //start the fun all over again.
    while(exit == 0){
        printf("Enter an input string: ");
        scanf("%[^\n]", input);
        if(strcmp(input, "EXIT") != 0){
            char* output = calloc(100, sizeof(char));
            output = removeDuplicates(input);
            char** tripleOutput = threeArrayStringSort(output);
            qsort(tripleOutput[0], strlen(tripleOutput[0]), sizeof(char), comparison);
            qsort(tripleOutput[1], strlen(tripleOutput[1]), sizeof(char), comparison);
            qsort(tripleOutput[2], strlen(tripleOutput[2]), sizeof(char), comparison);
            printf("\"");
            printf("%s%s%s", tripleOutput[0], tripleOutput[1], tripleOutput[2]);
            printf("\"\n");
            for(int i = 0; i < 2; i++){
                free(tripleOutput[i]);
            }
            free(output);
            clearInputBuffer();
        }else{
            exit = 1;
        }
    }
    return 0;
}
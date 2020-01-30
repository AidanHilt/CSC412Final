#include<stdlib.h>
#include<stdio.h>
#include<string.h>

/**
 * Given a char*, sorts it according to the requirements
 * given in the assignment, and returns a pointer to a
 * new string with the same characters sorted.
 */
char* stringSort(const char* input){
    //Getting the size of the input and allocating a new 
    //array to hold the output.
    int size = strlen(input);
    char* output = malloc(size * sizeof(char));

    //At this stage, we'll be looping through the array
    //once, to count the number of letters and numbers 
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
    
    //We'll start placing the letters at index 0, the numbers at the index
    //beginning at the letter count, and then the index for other characters
    //at the letter count plus the number count.
    int letterIndex = 0;
    int numIndex = letterIndex + letterCount;
    int otherIndex = numIndex + numCount;

    //We're going to go through the input string again. This time, when we check if something is a letter
    //or number, we're going to put it in the output array, at the appropriate index, then increment the 
    //variable storing the correct index so the program knows where the item goes. If it's neither, then
    //we'll do the same with the otherIndex variable.
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
            char* finalOutput = stringSort(output);
            printf("\"");
            printf("%s", finalOutput);
            printf("\"\n");
            free(output);
            clearInputBuffer();
        }else{
            exit = 1;
        }
    }
    return 0;
}
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

const int RANGE = 1;
const int LIST = 2;

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

struct typeRange{
    char* name;
    char* lowerBounds;
    char* upperBounds;
    int length;
    int traversalMode;
};

struct typeRangeAggregate{
    struct typeRange* types;
    int typeCount;
};

//More or less the same as tripleArraySort(), but it dynamically allocates things
//on the heap, based on the typeCount of ranges
char** arbitraryArrayStringSort(char* input, struct typeRangeAggregate ranges){
    int size = strlen(input);

    int* typeCounts = calloc(ranges.typeCount + 1, sizeof(int));

    //Yeah, yeah, yeah, it's a triple nested for loop. Don't get your panties in a twist, because the length of a)
    //the number of types for sorting, and b) the number of categories they each have shouldn't be too long. This
    //does help to allow it to take an arbitrary number of categories, as long as the structs are properly initialized.
    //This one counts the number of each category
    
    for(int i = 0; i < size; i++){
        for(int j = 0; j < ranges.typeCount; j++){
            for(int k = 0; k < ranges.types[j].length; k++){
                
                if(ranges.types[j].traversalMode == RANGE){
                    if(input[i] >= ranges.types[j].lowerBounds[k] && input[i] <= ranges.types[j].upperBounds[k]){
                        typeCounts[j] ++;
                    }
                }

                if(ranges.types[j].traversalMode == LIST){
                    if(ranges.types[j].lowerBounds[k] == input[i]){
                        typeCounts[j] ++;
                    }
                }

            }
        }
    }

    char** outputs = calloc(ranges.typeCount + 1, sizeof(char*));
    int* currentIndices = calloc(ranges.typeCount + 1, sizeof(int));
    int finalIndex = 0;

    /*So here, we're allocating space for each of the char*s in the output
     *char**. The last one is separate because I gave it custom logic to avoid
     *having to have an extra category. 
     */
    for(int i = 0; i < ranges.typeCount; i++){
        outputs[i] = calloc(typeCounts[i], sizeof(char));
    }

    outputs[ranges.typeCount] = calloc(size - finalIndex, sizeof(char));

    /**
     * Same here. Triple for loop, it's here, get over it. Here we're gonna check if each character in the input
     * string is a valid member of any of the categories, and if so, do the appending. If not, throw it in 
     * with the rest of the "others"
     */
    for(int i = 0; i < size; i++){
        int homeFound = 0;
        for(int j = 0; j < ranges.typeCount; j++){
            for(int k = 0; k < ranges.types[j].length; k++){
                
                if(ranges.types[j].traversalMode == RANGE){
                    if(input[i] >= ranges.types[j].lowerBounds[k] && input[i] <= ranges.types[j].upperBounds[k]){
                        homeFound = 1;
                        outputs[j][currentIndices[j]] = input[i];
                        currentIndices[j]++;
                    }
                }

                if(ranges.types[j].traversalMode == LIST){
                    if(ranges.types[j].lowerBounds[k] == input[i]){
                        homeFound = 1;
                        outputs[j][currentIndices[j]] = input[i];
                        currentIndices[j]++;
                    }
                }
            }
        }
        if(homeFound == 0){
            outputs[ranges.typeCount][currentIndices[ranges.typeCount]] = input[i];
            currentIndices[ranges.typeCount] ++;
        }
    }

    return outputs;
}

/**
 * Gets an array of c-strings, and the length
 * of that array. Then, it loops through the 
 * c-string array, and calls qsort on each
 * character array.
 */
void stringQSorter(char** input, int len){
    for(int i = 0; i < len; i++){
        qsort(input[i], strlen(input[i]), sizeof(char), comparison);
    }
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
    int exit = 0;
    char input[101];

    struct typeRangeAggregate aggregate = {.typeCount = 3};
    aggregate.types = calloc(3, sizeof(struct typeRange));

    /**
    * So, mostly the same as the last one. Obviously, we have to initialize our structs,
    * I respect that there is almost certainly a more elegant way to do it, but frankly,
    * I don't care. 
    */

    aggregate.types[0].name = "Numbers";
    aggregate.types[0].lowerBounds = "0";
    aggregate.types[0].upperBounds = "9";
    aggregate.types[0].length = 1;
    aggregate.types[0].traversalMode = RANGE;

    aggregate.types[1].name = "Mathematical Operators";
    aggregate.types[1].lowerBounds = "+-*/=%!";
    aggregate.types[1].length = 7;
    aggregate.types[1].traversalMode = LIST;

    aggregate.types[2].name = "Letters";
    aggregate.types[2].lowerBounds = "Aa";
    aggregate.types[2].upperBounds = "Zz";
    aggregate.types[2].length = 2;
    aggregate.types[2].traversalMode = RANGE;

    while(exit == 0){
        printf("Enter an input string: ");
        scanf("%[^\n]s", input);
        if(strcmp(input, "EXIT") != 0){
            char* output = calloc(100, sizeof(char));
            output = removeDuplicates(input);
            char** tripleOutput = arbitraryArrayStringSort(output, aggregate);
            stringQSorter(tripleOutput, 4);
            printf("\"");
            for (int i = 0; i < 4; i++){
                printf("%s", tripleOutput[i]);
            }
            printf("\"\n");
            free(output);
            for(int i = 0; i < 2; i++){
                free(tripleOutput[i]);
            }
            free(tripleOutput);
            clearInputBuffer();
        }else{
            exit = 1;
        }
    }
    return 0;
}
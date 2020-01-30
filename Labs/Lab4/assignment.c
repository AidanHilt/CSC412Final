#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct category{
    char* name;
    int len;
    char** contents;
};

FILE * readFile(const char* file){
    FILE* readFile = fopen(file, "r");

    if(readFile == NULL){
        printf("Couldn't open provided file\n");
        exit(1);
    }else{
        return readFile;
    }
}

struct category* processFile(FILE* readFile){
    int numCats;

    int totalRead = fscanf(readFile, "%d", &numCats);

    if(totalRead != 1){
        printf("Couldn't get the number of categories correctly\n");
        exit(2);
    }

    struct category* fileContents = calloc(numCats, sizeof(struct category*));

    char* name;
    int numWords;

    for(int i = 0; i < numCats; i++){
        if(fscanf(readFile, "%s %d", name, numWords) == 2){
            fileContents[i].name = name;
            fileContents[i].len = numWords;
            fileContents[i].contents = calloc(numWords, sizeof(char*));
           
            char* read;

            for(int j = 0; j < numWords; j++){
                if(fscanf(readFile, "%s ") == 1){
                    fileContents[i].contents[];
                }
            }
        }else{
            printf("Invalid format for data in %s. Exiting now\n");
            exit(3);
        }
    }
}

int main(int argc, char* argv[]){

}
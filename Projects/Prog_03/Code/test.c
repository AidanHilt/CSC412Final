#include <stdlib.h>        
#include <stdio.h>
#include <string.h>

#include "ImageIO/imageIO_TGA.c"

/**
 * char* outputDirectory: The directory where we want to place our file
 * char* fileName: The name of the file we will be writing, so we can check 
 * if things exist
 */
int determineFileNumber(char* outputDirectory, char* fileName){
    char* dot = strrchr(fileName, '.');
    char* noExtensionFileName = calloc(strlen(fileName), sizeof(char));
    memcpy(noExtensionFileName, fileName, dot - fileName);

    char* universal = calloc(strlen(outputDirectory) + strlen(noExtensionFileName) + 2, sizeof(char));

    strcat(universal, outputDirectory);
    strcat(universal, "/");
    strcat(universal, noExtensionFileName);

    char *check = calloc(strlen(universal) + strlen(dot) + 13, sizeof(char));
    strcat(check, universal);
    strcat(check, " [cropped]");
    strcat(check, dot);

    FILE *file;

    if((file = fopen(check, "r")) != NULL){
        char *renameVal =  calloc(strlen(universal) + 11 + strlen(dot), sizeof(char));
        strcat(renameVal, universal);
        strcat(renameVal, " [cropped1]");
        strcat(renameVal, dot);
        rename(check, renameVal);
        fclose(file);
        free(renameVal);
    }

    free(check);

    for(int i = 1; i < 100; i++){
        char designation[20];
        sprintf(designation, " [cropped%d]", i);
        char *test = calloc(strlen(universal) + strlen(dot) + strlen(designation), sizeof(char));
        strcat(test, universal);
        strcat(test, designation);
        strcat(test, dot);
        if((file = fopen(test, "r")) == NULL){
            free(test);
            free(universal);
            free(noExtensionFileName);
            return i;
        }
        free(test);
        fclose(file);
    }
    free(universal);
    free(noExtensionFileName);
    return 100;
}

int main(int argc, char* argv[]){
    (void) argc;
    (void) argv;
    int test = determineFileNumber("../Output", "test.tga");
    printf("%d\n", test);
}


/**
 * @file dispatcher.c
 * @brief The program that takes a long list of arguments,
 * and forks these out to the given image operators 
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>

#include "Image_IO/imageIO_TGA.h"
#include "Image_IO/RasterImage.h"

#define DIM_NAME "dimensions"
#define CROP_NAME "crop"
#define SPL_NAME "split"
#define ROT_NAME "rotate"


/**
 * @param image The filename of the image to be modified. 
 * @param directory The name of the directory we want to append
 * onto the image name
 * @return A new char* with the value of image appended to the 
 * value of directory
 */
char* addDirectoryToImage(char* image, char* directory){
    char* imageWDirectory = calloc(strlen(image), sizeof(char));
    strcat(imageWDirectory, directory);
    strcat(imageWDirectory, image);
    return imageWDirectory;
}

/**
 * Deletes the executables built by buildExecutables()
 */
void deleteExecutables(){
    system("rm dimensions");
    system("rm crop");
    system("rm rotate");
    system("rm split");
}

/**
 * Builds the executables from source files in the same directory
 */
void buildExecutables(){
    system("gcc -o dimensions dimensions.c Image_IO/imageIO_TGA.c Image_IO/RasterImage.c");
    system("gcc -o crop crop.c Image_IO/imageIO_TGA.c Image_IO/RasterImage.c");
    system("gcc -o rotate rotate.c Image_IO/imageIO_TGA.c Image_IO/RasterImage.c");
    system("gcc -o split split.c Image_IO/imageIO_TGA.c Image_IO/RasterImage.c");
}

/**
 * @param executable The name of the executable to run
 * @param args The command line arguments to be passed
 * to the executable
 * @return 1 if the exectuable returned a non-zero, 
 * meaning it succeeded and we should increment the count
 * of failed executables, and 0 otherwise
 */
int dispatchOperator(char* executable, char** args){
    (void) executable;
    int* status = (int*)(calloc(1, sizeof(int)));
    if(fork() == 0){
        printf("%d\n", execv(executable, args));
        exit(3);
    }else{
        wait(status);
        if(*status != 0) return 1;
        free(status);
    }
    return 0;
}

/**
 * @param input A string that we want to check to see
 * if it is an executable name
 * @return An int n, such that n >= 1 if the string is an 
 * executable, and n <= 0 if it is not
 */
int isExecutable(char *input){
    if(strcmp(input, DIM_NAME) == 0) return 1;
    else if(strcmp(input, CROP_NAME) == 0) return 1;
    else if(strcmp(input, SPL_NAME) == 0) return 1;
    else if(strcmp(input, ROT_NAME) == 0) return 1;
    else return 0;
}

/**
 * @param argv Should be argv from main, but its a list with 
 * desired arguments in it
 * @param startPos The position that the args list should start 
 * from. This is inclusive
 * @param argc The total number of arguments in argv, i.e. argc
 * from main. This is used to prevent infinite loops
 * @return Returns a char* array of arguments from the starting
 * position up the the next executable
 */
char** buildArgsList(char** argv, int startPos, int argc, int* modPos, 
                    char* imageDirectory, char* outputDirectory){
    int lastPos = startPos + 1;

    while(lastPos < argc && !(isExecutable(argv[lastPos]))) lastPos ++;
    char** argsList = (char**)(calloc(lastPos - startPos + 2, sizeof(char*)));

    for(int i = 0; i < lastPos - startPos + 1; i++){
        argsList[i] = (char*)(calloc)(100, sizeof(char));
    }

    argsList[lastPos - startPos + 1] = NULL;

    for(int i = 0; i < lastPos - startPos; i++){
        if(strstr(argv[i + startPos], ".tga") != NULL){
            char* modifiedName = addDirectoryToImage(argv[i + startPos], imageDirectory);
            free(argsList[i]);
            argsList[i] = modifiedName;
        }else{
            argsList[i] = argv[i + startPos];
        }
    }

    argsList[lastPos - startPos] = outputDirectory;

    *modPos = --lastPos;
    return argsList;
}

/**
 * @param argc The number of arguments provided 
 * @param argv A list of arguments in the form of c-strings,
 * with the first one being the name of the executable
 * @return A return code representing the number of
 * dispatched commands that failed
 */
int main(int argc, char* argv[]){
    buildExecutables();
    int failureAccumulator = 0;
    for(int i = 3; i < argc; i++){
        if(isExecutable(argv[i])){
            int nextIndex = 0;
            char** args = buildArgsList(argv, i, argc, &nextIndex, argv[1], argv[2]);
            failureAccumulator += dispatchOperator(argv[i], args);
            free(args);
            i = nextIndex;
        }
    }
    deleteExecutables();
    return failureAccumulator;
}
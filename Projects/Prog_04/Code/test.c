#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "Image_IO/imageIO_TGA.h"
#include "Image_IO/RasterImage.h"

#define DIM_NAME "dimensions"
#define CROP_NAME "crop"
#define SPL_NAME "split"
#define ROT_NAME "rotate"

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

int main(int argc, char* argv[]){
    printf("%d\n", isExecutable("rotate"));
}
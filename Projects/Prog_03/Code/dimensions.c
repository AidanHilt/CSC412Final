#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ImageIO/imageIO_TGA.c"

/**
 * RasterImage image: A RasterImage whose dimensions are supposed to be printed
 * int reportWidth: If <= 0, indicates we should not report the width, and 
 * vice-versa if the reverse is true
 * int reportHeight: If <=0, indicates we should not report the height, and
 * vice-versa if the reverse is true
 * int verbose: If <=0, indicates that we should not use the verbose reporting
 * style, and vice-versa if the reverse is true
 */
void giveImageDimensions(RasterImage image, int reportWidth, int reportHeight,
                        int verbose){
    
    if(reportWidth > 0 && verbose <= 0){
        printf("%d", image.numColumns);
    }else if(reportWidth > 0 && verbose > 0){
        printf("width: %d", image.numColumns);
    }

    if(verbose > 0){
        printf(", ");
    }else{
        printf(" ");
    }

    if(reportHeight > 0 && verbose <= 0){
        printf("%d", image.numRows);
    }else if(reportHeight > 0 && verbose > 0){
        printf("height: %d", image.numRows);
    }

    printf("\n");
}


/**
 * Reads in a file, structures an image from it, and then gives the image
 * dimensions, all based on the command line arguments
 */
int main(int argc, char* argv[]){
    writeLog(argv, LOGFILENAME, argc);

    if(argc == 1 || argc > 4){ 
        char* OUTPUT[2] = {" ", "An incorrect number of arguments was provided, leading to program termination.\n"};
        reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
        exit(-4);
    }

    int reportWidth = 1;
    int reportHeight = 1;
    int verbose = 0;
    char* file;
    int widthTripped = 0;
    int heightTripped = 0; 
    int verboseTripped = 0;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-w") == 0){
            if(reportHeight == 1 && heightTripped == 0){
                reportHeight = 0;
                widthTripped++;
            }else{
                if(heightTripped == 0){ 
                    char* OUTPUT[2] = {" ", "You cannot use the '-w' argument twice.\n"};
                    reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
                }
                else{ 
                    char* OUTPUT[2] = {" ", "You cannot use the '-h' and the '-w' options together.\n"};
                    reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
                }
                exit(-3);
            }
        }

        else if(strcmp(argv[i], "-h") == 0){
            if(reportWidth == 1 && widthTripped == 0){
                reportWidth = 0;
                heightTripped++;
            }else{
                if(widthTripped == 0){ 
                    char* OUTPUT[2] = {" ", "You cannot use the '-h' argument twice.\n"};
                    reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
                }
                else{
                    char* OUTPUT[2] = {" ", "You cannot use the '-h' and the '-w' options together.\n"};
                    reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
                }
                exit(-3);
            }
        }

        else if(strcmp(argv[i], "-v") == 0){
            if(verboseTripped <= 0){
                verbose = 1;
                verboseTripped++;
            }else{
                char* OUTPUT[2] = {" ", "You cannot use the '-v' argument twice.\n"};
                reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
            }
        }

        else if(strstr(argv[i], ".tga") != NULL){
            file = argv[i];
        }

        else{
            char* OUTPUT[5] = {" ", "'", argv[i], "'", "is an invalid argument. Please consult the documentation for more information.\n"};
            reportAndWriteError(OUTPUT, 5, LOGFILENAME, 1);
            exit(-4);
        }
    }

    if(file != NULL){
        RasterImage image = structureImageFromFile(file);
        giveImageDimensions(image, reportWidth, reportHeight, verbose);
        char* OUTPUT[1] = {"\n"};
        writeLog(OUTPUT, LOGFILENAME, 1);
        clearRasterImage(image);
        exit(0);
    }else{
        char* OUTPUT[2] = {" ", "I have no idea what went wrong. Aborting...\n"};
        printf("%s", OUTPUT[1]);
        writeLog(OUTPUT, LOGFILENAME, 2);
        exit(-6);
    }
}
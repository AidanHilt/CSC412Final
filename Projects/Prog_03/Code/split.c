#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

#include "ImageIO/imageIO_TGA.c"

const int RED = 0;
const int GREEN = 1;
const int BLUE = 2;


/**
 * RasterImage image: The image whose color channel should be wiped
 * int colorIndex: The color that should be wiped. Represented by the 
 * integer constants RED, GREEN, and BLUE.
 * Returns an unsigned char* representing a raster for an image with 
 * the specified color channel canceled.
 */
unsigned char* wipeColor(unsigned char* raster, int colorIndex,
                        int numRows, int numColumns, int bytesPerRow){
    unsigned char* colorWipedRaster = calloc(numRows * numColumns * 4, sizeof(char));
    memcpy(colorWipedRaster, raster, numColumns * numRows * 4);

    for(int i = 0; i < numRows; i++){
        for(int j = 0; j < numColumns; j++){
            colorWipedRaster[i * bytesPerRow + 4 * j + colorIndex] = 0x00;
        }
    }
    return colorWipedRaster;
}

RasterImage createRedIsolatedImage(RasterImage image){
    unsigned char* noGreenImage = wipeColor(image.raster, GREEN, image.numRows, 
                                            image.numColumns, image.bytesPerRow);
    unsigned char* finalImage = wipeColor(noGreenImage, BLUE, image.numRows,
                                        image.numColumns, image.bytesPerRow);
    RasterImage redIsolatedImage = {finalImage, image.numColumns, image.numRows,
                                 image.bytesPerPixel, image.bytesPerRow};
    free(noGreenImage);
    return redIsolatedImage;
}

RasterImage createGreenIsolatedImage(RasterImage image){
    unsigned char* noRedImage = wipeColor(image.raster, RED, image.numRows, 
                                            image.numColumns, image.bytesPerRow);
    unsigned char* finalImage = wipeColor(noRedImage, BLUE, image.numRows,
                                        image.numColumns, image.bytesPerRow);
    RasterImage greenIsolatedImage = {finalImage, image.numColumns, image.numRows,
                                 image.bytesPerPixel, image.bytesPerRow};
    free(noRedImage);
    return greenIsolatedImage;
}

RasterImage createBlueIsolatedImage(RasterImage image){
    unsigned char* noRedImage = wipeColor(image.raster, RED, image.numRows, 
                                            image.numColumns, image.bytesPerRow);
    unsigned char* finalImage = wipeColor(noRedImage, GREEN, image.numRows,
                                        image.numColumns, image.bytesPerRow);
    RasterImage greenIsolatedImage = {finalImage, image.numColumns, image.numRows,
                                 image.bytesPerPixel, image.bytesPerRow};
    free(noRedImage);
    return greenIsolatedImage;
}

/**
 * const char* path: C-string representing the path we want to break a file name from
 * char* extension: C-string that will be made to hold the file extension
 * Given a path, returns a pointer that points to whatever comes after the last
 * slash. Also modifies the extension argument to point to the last period in the 
 * path, which should provide the extension
 */
char* breakFileNameFromPath(const char* path, char** extension){
    char* fileName = strrchr(path, '/');
    if(fileName == NULL) memcpy(fileName, path, strlen(path));
    else fileName++;
    *extension = strrchr(path, '.');
    return fileName;
}


/**
 * char* outputPath: char* that stores the path where the file is to be output
 * char* filename: The name of the file
 * char* fileExtension: The extension of the file.
 * Given the above arguments, creates an out path with the [cropped] designation
 */
char* buildOutputFilePath(const char* outputPath, const char* filePath, int colorCode){
    char* extension;
    char* fileName = breakFileNameFromPath(filePath, &extension);
    int directorySlash = 0;

    unsigned int outputPathLength = strlen(outputPath);
    if(outputPath[outputPathLength - 1] == '/') directorySlash = 1;
    unsigned int fileNameLength = strlen(fileName);
    unsigned int extensionLength = strlen(extension);

    char* output;

    if(directorySlash == 1) output = (char*) calloc(outputPathLength + fileNameLength + extensionLength + 2, sizeof(char));
    else output = (char*) calloc(outputPathLength + fileNameLength + extensionLength + 3, sizeof(char));

    strncpy(output, outputPath, outputPathLength);
    if(directorySlash == 0) strncat(output, "/", 2);
    strncat(output, fileName, fileNameLength - extensionLength);
    switch(colorCode){
        case 0:
            strncat(output, "_r", 3);
            break;
        case 1:
            strncat(output, "_g", 3);
            break;
        case 2:
            strncat(output, "_b", 3);
            break;
    }
    strncat(output, extension, extensionLength);

    return output;
}

/**
 * Takes as command line arguments the path to a TGA file, and an output directory,
 * and then splits the input image into three color channels and writes them into
 * the output directory
 */
int main(int argc, char* argv[]){
    writeLog(argv, LOGFILENAME, argc);

    if(argc != 3){
        char* OUTPUT[2] = {" ", "An incorrect number of arguments was provided, leading to program termination.\n"};
        reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
        exit(-4);
    }

    if(checkValidDirectory(argv[2]) <= 0){
        char* OUTPUT[2] = {" ", "The provided output directory is invalid. Aborting\n"};
        reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
        exit(-1);
    }

    RasterImage originalImage = structureImageFromFile(argv[1]);

    RasterImage redImage = createRedIsolatedImage(originalImage);
    RasterImage greenImage = createGreenIsolatedImage(originalImage);
    RasterImage blueImage = createBlueIsolatedImage(originalImage);

    char* redPath = buildOutputFilePath(argv[2], argv[1], RED);
    char* greenPath = buildOutputFilePath(argv[2], argv[1], GREEN);
    char* bluePath = buildOutputFilePath(argv[2], argv[1], BLUE);

    writeTGA(redPath, redImage.raster, redImage.numRows, redImage.numColumns);
    writeTGA(greenPath, greenImage.raster, greenImage.numRows, greenImage.numColumns);
    writeTGA(bluePath, blueImage.raster, blueImage.numRows, blueImage.numColumns);

    free(redPath);
    free(greenPath);
    free(bluePath);

    clearRasterImage(originalImage);
    clearRasterImage(redImage);
    clearRasterImage(greenImage);
    clearRasterImage(blueImage);

    char* OUTPUT[1] = {"\n"};

    writeLog(OUTPUT, LOGFILENAME, 1);
}
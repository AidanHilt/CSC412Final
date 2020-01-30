#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ImageIO/imageIO_TGA.c"

/**
 * RasterImage image: The image whose cropping boundaries are to be checked
 * int x: The x coordinate representing the upper-left corner of the desired cropping area
 * int y: The y coordinate representing the upper-left corner of the desired cropping area
 * int width: The width(x-axis) of the desired cropping area
 * int height: The height(y-axis) of the desired cropping area
 * Using the given RasterImage, and the given bounds, checks that they are valid bounds for
 * a crop operation. Returns > 0 if it is valid, and <= 0 if not.
 */
int checkCropBounds(RasterImage image, int x, int y, int width, int height){
    if(x + width >= image.numColumns) return 0;
    if(y + height >= image.numRows) return 0;
    return 1;
}


/**
 * RasterImage original: The RasterImage struct that holds the image
 * we want to crop
 * unsigned int x: The x coordinate of the top left corner of the cropping
 * rectangle
 * unsigned int y: The y coordinate of the top left corner of the cropping
 * rectangle
 * unsigned int width: The width of the cropping rectangle
 * unsigned int height: The height of the cropping rectangle
 * Given an input RasterImage, outputs a RasterImage representing a cropped
 * section of the input, with all of the field appropriately modified.
 */
RasterImage createCroppedImage(RasterImage original, unsigned int x, unsigned int y, unsigned int width,
                              unsigned int height){
    unsigned char* croppedRaster = calloc(width * height * original.bytesPerPixel, sizeof(char));
    int newBytesPerRow = width * original.bytesPerPixel;

    for(unsigned int i = 0; i < height; i++){
        for(unsigned int j = 0; j < width; j++){
            int originalRasterRow = (original.numRows - (i + y) - 1) * original.bytesPerRow;
            int newRasterRow = (height - i - 1) * newBytesPerRow;

            croppedRaster[newRasterRow + j * original.bytesPerPixel] = 
                original.raster[originalRasterRow + (j + x) * original.bytesPerPixel];
            croppedRaster[newRasterRow + j * original.bytesPerPixel + 1] = 
                original.raster[originalRasterRow + (j + x) * original.bytesPerPixel + 1];
            croppedRaster[newRasterRow + j * original.bytesPerPixel + 2] =
                original.raster[originalRasterRow + (j + x) * original.bytesPerPixel + 2];
        }
    }

    RasterImage finalImage = {croppedRaster, width, height,
                            original.bytesPerPixel, newBytesPerRow};

    return finalImage;
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
 * char* outputDirectory: The directory where we want to place our file
 * char* fileName: The name of the file we will be writing, so we can check 
 * if things exist
 */
int determineFileNumber(const char* outputDirectory, const char* fileName){
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

/**
 * char* outputPath: char* that stores the path where the file is to be output
 * char* filename: The name of the file
 * char* fileExtension: The extension of the file.
 * Given the above arguments, creates an out path with the [cropped] designation
 */
char* buildOutputFilePath(const char* outputPath, const char* filePath){
    char* extension;
    char* fileName = breakFileNameFromPath(filePath, &extension);
    char designation[20];
    int fileNumber = determineFileNumber(outputPath, fileName);
    sprintf(designation, " [cropped%d]", fileNumber);
    int directorySlash = 0;

    unsigned int outputPathLength = strlen(outputPath);
    if(outputPath[outputPathLength - 1] == '/') directorySlash = 1;
    unsigned int fileNameLength = strlen(fileName);
    unsigned int designationLength = strlen(designation);
    unsigned int extensionLength = strlen(extension);

    char* output;

    if(directorySlash == 1) output = (char*) calloc(outputPathLength + fileNameLength + 
                            designationLength + extensionLength, sizeof(char));
    else output = (char*) calloc(outputPathLength + fileNameLength + designationLength + 
                            extensionLength + 1, sizeof(char));

    strncpy(output, outputPath, outputPathLength);
    if(directorySlash == 0) strncat(output, "/", 2);
    strncat(output, fileName, fileNameLength - extensionLength);
    strncat(output, designation, designationLength);
    strncat(output, extension, extensionLength);

    return output;
}

/**
 * Takes command line input that contains a valid TGA file, an output directory,
 * the x and y coordinates of the top left corner of the cropping rectangle,
 * and the length and height of the cropping rectangle.
 */
int main(int argc, char* argv[]){
    writeLog(argv, LOGFILENAME, argc);
    if(argc != 7){
        char* OUTPUT[2] = {" ", "An incorrect number of arguments was provided.\n"};
        reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
    }

    if(checkValidDirectory(argv[2]) <= 0){
        char* OUTPUT[2] = {" ", "The provided output directory is invalid. Aborting\n"};
        reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
        exit(-1);
    }

    RasterImage originalImage = structureImageFromFile(argv[1]);

    int x = atoi(argv[3]);
    int y = atoi(argv[4]);
    int width = atoi(argv[5]);
    int height = atoi(argv[6]);

    if(checkCropBounds(originalImage, x, y, width, height) <= 0){
        char* OUTPUT[2] = {" ", "The provided cropping bounds are not valid for the input image\n"};
        reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
        exit(-1);
    }

    RasterImage croppedImage = createCroppedImage(originalImage,x, y, width, height);

    clearRasterImage(originalImage);

    writeTGA(buildOutputFilePath(argv[2], argv[1]), croppedImage.raster, croppedImage.numRows, croppedImage.numColumns);

    clearRasterImage(croppedImage);

    char* OUTPUT[1] = {"\n"};

    writeLog(OUTPUT, LOGFILENAME, 1);
}
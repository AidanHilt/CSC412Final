#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ImageIO/imageIO_TGA.c"

/**
 * char* rotationString: A string containing a series of ls and rs that specify
 * a series of rotation operations to perform on an image
 * Given a rotation string, returns an integer that specifies the number and direction
 * of rotations that should be performed. The magnitude of the number specifies how many
 * times the rotation operation should be performed, with the sign specifying the direction
 * (negative = left rotation, positive = right rotation)
 */
int calculateRotationFromString(char* rotationString){
    size_t stringLength = strlen(rotationString);
    int leftOps = 0, rightOps = 0;

    for(unsigned int i = 0; i < stringLength; i++){
        if(rotationString[i] == 'l' || rotationString[i] == 'L') leftOps++;
        else if(rotationString[i] == 'r' || rotationString[i] == 'R') rightOps++;
        else{
            char ERROR_STRING[112];
            sprintf(ERROR_STRING, "The incorrect character '%c' was detected in the rotation string. Only upper or lowercase L or R should be used\n", rotationString[i]);
            char* OUTPUT[2] = {" ", ERROR_STRING};
            reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
            exit(-1);
        }
    }

    int totalOps = rightOps - leftOps;

    if(totalOps >= 4 || totalOps <= -4) totalOps = totalOps % 4;
    if(totalOps == 3) totalOps = -1;
    if(totalOps == -3) totalOps = 1;

    return totalOps;
}

/**
 * char *raster: The raster of the image to be left-rotated
 * int *width: The width of the current image. Will be overwritten with the height of the old image,
 * (i.e. the width of the rotated image) which is why it is passed as a pointer.
 * int *height: The height of the current image. Will be overwritten with the width of the old image,
 * (i.e. the height of the rotated image) which is why it is passed as a pointer.
 * int bytesPerPixel: The number of pytes each pixel occupies
 * Given an input rasterImage, and pointers to it's width and height, returns a char* representing the
 * raster of a 90 degree left-rotated version of the image, while also changing the pointers to 
 * reflect the new width and height of the image
 */
unsigned char* rotateImageLeft(unsigned char *raster, unsigned int *width, unsigned int *height, int bytesPerPixel){
    unsigned char* rotatedImage = calloc(*width * *height * bytesPerPixel, sizeof(char)); 
    unsigned int originalRasterBytesPerRow = *width * bytesPerPixel;
    unsigned int rotatedRasterBytesPerRow = *height * bytesPerPixel;

    for(unsigned int i = 0; i < *height; i++){
        for(unsigned int j = 0; j < *width; j++){
            
            rotatedImage[j * rotatedRasterBytesPerRow + i * bytesPerPixel] = 
                raster[(*height - i) * originalRasterBytesPerRow + j * bytesPerPixel];
            
            rotatedImage[j * rotatedRasterBytesPerRow + i * bytesPerPixel + 1] = 
                raster[(*height - i) * originalRasterBytesPerRow + j * bytesPerPixel + 1];
            
            rotatedImage[j * rotatedRasterBytesPerRow + i * bytesPerPixel + 2] = 
                raster[(*height - i) * originalRasterBytesPerRow + j * bytesPerPixel + 2];
            
        }
    }

    int temp = *width;
    *width = *height;
    *height = temp;

    return rotatedImage;
}

/**
 * char *image: The raster of the image to be right-rotated
 * int *width: The width of the current image. Will be overwritten with the height of the old image,
 * (i.e. the width of the rotated image) which is why it is passed as a pointer.
 * int *height: The height of the current image. Will be overwritten with the width of the old image,
 * (i.e. the height of the rotated image) which is why it is passed as a pointer. 
 * int bytesPerPixel: The number of bytes each pixel contains
 * Given an input rasterImage, and pointers to it's width and height, returns a char* representing the
 * raster of a 90 degree right-rotated version of the image, while also changing the pointers to 
 * reflect the new width and height of the image
 */
unsigned char* rotateImageRight(unsigned char *raster, unsigned int *width, unsigned int *height, int bytesPerPixel){
    unsigned char* rotatedImage = calloc(*width * *height * bytesPerPixel, sizeof(char)); 
    unsigned int originalRasterBytesPerRow = *width * bytesPerPixel;
    unsigned int rotatedRasterBytesPerRow = *height * bytesPerPixel;

    for(unsigned int i = 0; i < *height; i++){
        for(unsigned int j = 0; j < *width; j++){
            rotatedImage[(*width - j) * rotatedRasterBytesPerRow + i * bytesPerPixel] = 
                raster[i * originalRasterBytesPerRow + j * bytesPerPixel];
            rotatedImage[(*width - j) * rotatedRasterBytesPerRow + i * bytesPerPixel + 1] = 
                raster[i * originalRasterBytesPerRow + j * bytesPerPixel + 1];
            rotatedImage[(*width - j) * rotatedRasterBytesPerRow + i * bytesPerPixel + 2] = 
                raster[i * originalRasterBytesPerRow + j * bytesPerPixel + 2];
            
        }
    }

    int temp = *width;
    *width = *height;
    *height = temp;

    return rotatedImage;
}

/**
 * RasterImage image: The image to be rotated
 * int rotationLevel: How many times the image is to be rotated, and what direction it should be
 * rotated. Negative values indicate a left rotation, and positive values a right rotation
 * Given the specified image and the rotation value, returns a RasterImage representing an
 * image rotated that many times in the specified direction
 */
RasterImage rotateImage(RasterImage image, int rotationLevel){
    unsigned char* rotatedRaster = image.raster;
    unsigned int width = image.numColumns;
    unsigned int height = image.numRows;

    if(rotationLevel < 0){
        for(; rotationLevel < 0; rotationLevel++){
            free(rotatedRaster);
            rotatedRaster = rotateImageLeft(rotatedRaster, &width, &height, image.bytesPerPixel);
        }
        RasterImage outImage = {rotatedRaster, width, height, image.bytesPerPixel, width * image.bytesPerPixel};
        return outImage;
    }
    else if(rotationLevel > 0){
        for(; rotationLevel > 0; rotationLevel--){
            free(rotatedRaster);
            rotatedRaster = rotateImageRight(rotatedRaster, &width, &height, image.bytesPerPixel);
        }
        RasterImage outImage = {rotatedRaster, width, height, image.bytesPerPixel, width * image.bytesPerPixel};
        return outImage;
    }
    else{
        char* OUTPUT[2] = {" ", "Somehow, the function to calculate a rotation int from a string has returned a 0. You will get the same image as output\n"};
        reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
        return image;
    }

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
char* buildOutputFilePath(const char* outputPath, const char* filePath){
    char* extension;
    char* fileName = breakFileNameFromPath(filePath, &extension);
    char* designation = " [rotated]";
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
 * Takes a rotation string, the path to to a TGA file, and the output directory
 * from the command line. From there, it generates an appropriately rotated image,
 * then writes it to the output directory, with the ' [cropped]' designation.
 */
int main(int argc, char* argv[]){ 
    writeLog(argv, LOGFILENAME, argc);
    if(argc != 4){
        char* OUTPUT[2] = {" ", "An incorrect number of arguments was provided. Aborting\n"};
        reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
        exit(-1);
    }

    if(checkValidDirectory(argv[3]) <= 0){
        char* OUTPUT[2] = {" ", "The provided output directory is invalid. Aborting\n"};
        reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
        exit(-1);
    }

    RasterImage inputImage = structureImageFromFile(argv[2]);
    int rotationCode = calculateRotationFromString(argv[1]);

    RasterImage rotatedImage = rotateImage(inputImage, rotationCode);

    char* outputPath = buildOutputFilePath(argv[3], argv[2]);

    writeTGA(outputPath, rotatedImage.raster, rotatedImage.numRows, rotatedImage.numColumns);

    free(outputPath);

    clearRasterImage(inputImage);
    clearRasterImage(rotatedImage);

    char* OUT[1] = {"\n"};
    writeLog(OUT, LOGFILENAME, 1);
}
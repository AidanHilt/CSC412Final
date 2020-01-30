/**
 * @file
 * @brief The file for tile.c, which takes a list of paths to image files,
 * and an output file name, and tiles them together
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "Image_IO/imageIO_TGA.h"
#include "Image_IO/RasterImage.h"

/**
 * @param imageWidth The width of all the given images
 * @param imageHeight The height of all the given images
 * @param numImages The total number of images given as input
 * @return Returns the width and height of the final image, given
 * in original images. (I.e., if given 12 images, then the result
 * will be 4, because 4 images by 4 images will be the smallest that
 * can fit)
 */
int determineDimensions(int numImages){
    double root = sqrt(numImages);
    return (int)ceil(root);
}

/**
 * @param image The RasterImage that we will be writing into a final image
 * @param imageX The x coordinate the image will be written to. This is measured
 * in units corresponding to a single image (so, if we want to write the second
 * image in the first row, the coordinate are (0, 1))
 * @param imageY The y coordinate the image will be written to. This is measured
 * in units corresponding to a single image (so, if we want to write the second
 * image in the first row, the coordinate are (0, 1))
 * @param finalRaster The raster that all these images are being written to
 */
void writeImageToFinalImage(RasterImage image, int imageX, int imageY, RasterImage newImage){
    int** oldRaster = (int**)image.raster2D;
    int** newRaster = (int**)newImage.raster2D;
    for(unsigned int i = 0; i < image.numRows; i++){
        for(unsigned int j = 0; j < image.numCols; j++){
            newRaster[imageY * image.numRows + i][imageX * image.numCols + j] = oldRaster[i][j];
        }
    }
}

/**
 * @param argc The number of arguments provided 
 * @param argv A list of arguments in the form of c-strings,
 * with the first one being the name of the executable
 * @return A return code
 */
int main(int argc, char* argv[]){
    int numImages = argc-2;

    RasterImage image = readTGA(argv[1]);

    int dimensionInImages = determineDimensions(numImages);
    int dimensionInPixels = dimensionInImages * ((image.numCols > image.numRows) ? image.numCols : image.numRows);

    RasterImage finalImage = newRasterImage(dimensionInPixels, dimensionInPixels, RGBA32_RASTER);

    int argvIndex = 1;

    for(int i = 0; i < dimensionInImages && argvIndex < argc -1; i++){
        for(int j = 0; j < dimensionInImages && argvIndex < argc -1; j++){
            deleteRasterImage(&image);
            image = readTGA(argv[argvIndex]);
            writeImageToFinalImage(image, j, i, finalImage);
            argvIndex++;
        }
    }

    writeTGA(argv[argc - 1], &finalImage);

    deleteRasterImage(&image);
    deleteRasterImage(&finalImage);
}

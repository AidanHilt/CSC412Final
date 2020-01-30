//
//  crop.c
//  Prog 02 Solution
//
//  Created by Jean-Yves Hervé on 2018-10-03.
//	Revised 2019-10-15
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//
#include "Image_IO/imageIO_TGA.h"

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Custom data types and global variables
//-------------------------------------------------------------------
#endif

/**	An enum type for all the errors that this program specifically handles
 */
typedef enum ErrorCode
{
	NO_ERROR = 0,
	//
	//	1x codes:	file-related	--> not used in this program
//	FILE_NOT_FOUND = 10,
//	CANNOT_OPEN_FILE = 11,
//	WRONG_FILE_TYPE = 12,
//	CANNOT_WRITE_FILE = 13,
	//
	//	2x codes:	command line argument errors
	WRONG_NUMBER_OF_ARGUMENTS = 20,
	INVALID_CROP_X_TYPE,
	INVALID_CROP_Y_TYPE,
	INVALID_CROP_CORNER,
	INVALID_CROP_WIDTH_TYPE,
	INVALID_CROP_HEIGHT_TYPE,
	INVALID_CROP_SIZE
	
} ErrorCode;


#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Function prototypes
//-------------------------------------------------------------------
#endif

/** interprets the program's input argument to determine the crop region.
 *	@param	argv	list of input argument to the program
 *	@param	cropCornerX		x coordinate of crop region's corner
 *	@param	cropCornerY		y coordinate of crop region's corner
 *	@param	cropWidth		crop region's width
 *	@param	cropHeight		crop region's height
 *	@return		an error code (0 if no error)
 */
int extractCropRegion(char* argv[],
					  unsigned int imageWidth, unsigned int imageHeight,
					  unsigned int* cropCornerX, unsigned int* cropCornerY,
					  unsigned int* cropWidth, unsigned int* cropHeight);

/**	Produces a new image that is a cropped part of the input image
 *	@param	image			the image to crop
 *	@param 	cropCornerX		x coordinate of the upper-left corner of the crop region
 *	@param 	cropCornerY		y coordinate of the upper-left corner of the crop region
 *							(counted from the top, so height - row index - 1
 *	@param 	cropWidth		width of the crop region
 *	@param 	cropHeight		height of the top region
 *	@return		the new image resulting from applying the crop
 */
RasterImage cropImage(RasterImage *image,
					  unsigned int cropCornerX, unsigned int cropCornerY,
					  unsigned int cropWidth, unsigned int cropHeight);

/**	In this app, just prints out an error message to the console and
 *	exits with the proper error code.  In a fancier version, could
 *	write to a log and "swallow" some non-critical errors.
 *
 *	@param code		the code of the error to report/process
 *	@param input	the input string that caused the error (NULL otherwise)
 */
void errorReport(ErrorCode code, char* input);


/**	Produces a complete path to the output image file.
 *	If the input file path was ../../Images/cells.tga and the
 *	and the output folder path is ../Output [with or without final slash),
 *	then the output file path will be ../Output/cells [cropped].tga
 *
 *	@param inputImagePath	path to the input image
 *	@param outFolderPath	path to the output folder
 *	@return	complete path to the desired output file.
 */
char* produceOutFilePath(char* inputImagePath, char* outFolderPath);

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Function implementations
//-------------------------------------------------------------------
#endif

//--------------------------------------------------------------
//	Main function, expecting as arguments:
//		inputImagePath outFolderPath x y width height
//	It returns an error code (0 for no error)
//--------------------------------------------------------------
int main(int argc, char* argv[])
{
	//	We need 6 arguments: filePath outputPath cornerX cornerY croppedW croppedW
	if (argc != 7)
	{
		printf("Proper usage: crop inputImagePath outFolderPath x y width height\n");
		return WRONG_NUMBER_OF_ARGUMENTS;
	}
	
	//	Just to look prettier in the code, I give meaningful names to my arguments
	char* inputImagePath = argv[5];
	char* outFolderPath = argv[6];

	//	Read the image
	RasterImage image = readTGA(inputImagePath);

	//	extract the parameters of the crop region
	unsigned int cropCornerX, cropCornerY, cropWidth, cropHeight;
	extractCropRegion(argv, image.numCols, image.numRows,
					  &cropCornerX, &cropCornerY, &cropWidth, &cropHeight);
	
	//	Perform the cropping
	RasterImage croppedImage = cropImage(&image, cropCornerX, cropCornerY, cropWidth, cropHeight);

	// Produce the path to the output file
	char* outFilePath = produceOutFilePath(inputImagePath, outFolderPath);
	
	//	Write out the cropped image
	int err = writeTGA(outFilePath, &croppedImage);
	
	//	Cleanup allocations.  Again, this is not really needed, since the full
	//	partition will get cleared when the process terminates, but I like to
	//	keep the good habit of freeing memory that I don't need anymore, and,
	//	if I crash, it's a sign that something went wrong earlier and I may
	//	have produced junk
	deleteRasterImage(&image);
	deleteRasterImage(&croppedImage);
	free(outFilePath);
	
	return err;
}

int extractCropRegion(char* argv[],
					  unsigned int imageWidth, unsigned int imageHeight,
					  unsigned int* cropCornerX, unsigned int* cropCornerY,
					  unsigned int* cropWidth, unsigned int* cropHeight)
{
	if (sscanf(argv[1], "%u", cropCornerX) != 1)
		errorReport(INVALID_CROP_X_TYPE, argv[1]);

	if (sscanf(argv[2], "%u", cropCornerY) != 1)
		errorReport(INVALID_CROP_Y_TYPE, argv[2]);

	//	Note: since we read into an unsigned int, a negative value would come out
	//	as a large positive value
	if ((*cropCornerX >= imageWidth) || (*cropCornerY >= imageHeight))
		errorReport(INVALID_CROP_CORNER, NULL);

	if (sscanf(argv[3], "%u", cropWidth) != 1)
		errorReport(INVALID_CROP_WIDTH_TYPE, argv[3]);

	if (sscanf(argv[4], "%u", cropHeight) != 1)
		errorReport(INVALID_CROP_HEIGHT_TYPE, argv[3]);

	//	Note: since we read into an unsigned int, a negative value would come out
	//	as a large positive value
	if ((*cropCornerX + *cropWidth > imageWidth) ||
		(*cropCornerY + *cropHeight > imageHeight))
		errorReport(INVALID_CROP_SIZE, NULL);
	
	//	Otherwise, all is ok, go back to crop
	return 0;
}

RasterImage cropImage(RasterImage* imageIn,
					  unsigned int cropCornerX, unsigned int cropCornerY,
					  unsigned int cropWidth, unsigned int cropHeight)
{
	RasterImage imageOut = newRasterImage(cropHeight, cropWidth, RGBA32_RASTER);
	
	//	Beware that the images are stored upside-down from the way we view them,
	//	So I need to invert the row indices.
	for (unsigned int i = 0; i<cropHeight; i++)
	{
		//	version using the 1D rasters
		memcpy((unsigned char*) imageOut.raster + (imageOut.numRows - i - 1)*imageOut.bytesPerRow,
			   (unsigned char*) imageIn->raster + (imageIn->numRows - i - cropCornerY - 1)*imageIn->bytesPerRow
							   					+ cropCornerX * imageIn->bytesPerPixel,
			   cropWidth*imageIn->bytesPerPixel);

		//	version using the 2D rasters
//		memcpy(((unsigned char**) imageOut.raster2D)[imageOut.numRows - i - 1],
//			   ((unsigned char**) imageIn->raster2D)[imageIn->numRows - i - cropCornerY - 1]
//							   					+ cropCornerX * imageIn->bytesPerPixel,
//			   cropWidth*imageIn->bytesPerPixel);
		
	}

	return imageOut;
}

char* produceOutFilePath(char* inputImagePath, char* outFolderPath)
{
	// Produce the name of the output file
	//-------------------------------------
	//	First, find the start of the input file's name.  Start from the end
	//	and move left until we hit the first slash or the left end of the string.
	unsigned long index = strlen(inputImagePath) - 5;
	while ((index>=1) && (inputImagePath[index-1] != '/'))
		index--;
	
	//	Produce the name of the input file minus extension
	char* inputFileRootName = (char*) malloc(strlen(inputImagePath+index) +1);
	strcpy(inputFileRootName, inputImagePath+index);
	//	chop off the extension by replacing the dot by '\0'
	inputFileRootName[strlen(inputFileRootName)-4] = '\0';

	char* outFilePath = (char*) malloc(strlen(outFolderPath) +
										strlen(inputFileRootName) + strlen(" [cropped].tga") + 2);
	strcpy(outFilePath, outFolderPath);
	//	If outFolderPath didn't end with a slash, add it
	if (outFolderPath[strlen(outFolderPath)-1] != '/')
		strcat(outFilePath, "/");

	//	Append root name to output path, add " [cropped].tga"
	strcat(outFilePath, inputFileRootName);
	strcat(outFilePath, " [cropped].tga");
	
	//	free heap-allocated data we don't need anymore
	free(inputFileRootName);
	
	return outFilePath;
}

void errorReport(ErrorCode code, char* input)
{
	if (code != NO_ERROR)
	{
		switch (code)
		{
			case WRONG_NUMBER_OF_ARGUMENTS:
			break;
			
			case INVALID_CROP_X_TYPE:
				printf("Third argument is not a positive integer: %s\n", input);
			break;
			
			case INVALID_CROP_Y_TYPE:
				printf("Fourth argument is not a positive integer: %s\n", input);
			break;
			
			case INVALID_CROP_CORNER:
				printf("The crop region's upper-left corner must be within the image.\n");
			break;
			
			case INVALID_CROP_WIDTH_TYPE:
				printf("Fifth argument is not a positive integer: %s\n", input);
			break;
			
			case INVALID_CROP_HEIGHT_TYPE:
				printf("Sixth argument is not a positive integer: %s\n", input);
			break;
			
			case INVALID_CROP_SIZE:
			break;
			
			default:
				break;
		}
		exit(code);
	}
}



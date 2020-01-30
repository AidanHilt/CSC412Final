//
//  split.c
//  Prog 02 Solution
//
//  Created by Jean-Yves Hervé on 2018-11.21.
//	Revised 2019-10-14
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
	FILE_NOT_FOUND = 10,
	CANNOT_OPEN_FILE = 11,
	WRONG_FILE_TYPE = 12,
	CANNOT_WRITE_FILE = 13,
	//
	//	2x codes:	command line argument errors
	WRONG_NUMBER_OF_ARGUMENTS = 20
//	INVALID_CROP_X_TYPE,
//	INVALID_CROP_Y_TYPE,
//	INVALID_CROP_CORNER,
//	INVALID_CROP_WIDTH_TYPE,
//	INVALID_CROP_HEIGHT_TYPE,
//	INVALID_CROP_SIZE
	
} ErrorCode;

/**	An enum type to code the color channels of my image.
 *	I could do the same thing by hard-coding 0-1-2 (UUUUUgly!)
 *	or #define-ing RED_CHANNEL	0, etc.
 *	I use enum types as much as possible because I am a firm advocate of
 *	strong typing.
 */
typedef enum ColorChannel
{
	RED_CHANNEL = 0,
	GREEN_CHANNEL,
	BLUE_CHANNEL,
	//
	NUM_OF_CHANNELS		//	Automatically = 3
} ColorChannel;

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Function prototypes
//-------------------------------------------------------------------
#endif

/**	In this app, just prints out an error message to the console and
 *	exits with the proper error code.  In a fancier version, could
 *	write to a log and "swallow" some non-critical errors.
 *
 *	@param code		the code of the error to report/process
 *	@param input	the input string that caused the error (NULL otherwise)
 */
void errorReport(ErrorCode code, const char* input);

/**	Splits the input image into 3 separate images storing the red, green, and
 *	blue color channels
 *	@param  img	the image to split
 *	@return	an array of images storing red-green-blue channels
 */
RasterImage* splitImage(RasterImage* img);
 
/**	Produces a complete path to the each of the output image files.
 *	If the input file path was ../../Images/cells.tga and the
 *	and the output folder path is ../Output [with or without final slash),
 *	then the output file path will be ../Output/cells [r/g/b].tga
 *
 *	@param inputImagePath	path to the input image
 *	@param outFolderPath	path to the output folder
 *	@return	complete array of paths to the desired output files.
 */
char** produceOutFilePath(char* inputImagePath, char* outFolderPath);

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
	//	we need at least one argument
	if (argc != 3)
	{
		printf("Proper usage: split inputImagePath outFolderPath\n");
		return WRONG_NUMBER_OF_ARGUMENTS;
	}
	
	//	Just to look prettier in the code, I give meaningful names to my arguments
	char* inputImagePath = argv[1];
	char* outFolderPath = argv[2];

	//	Read the image
	RasterImage image = readTGA(inputImagePath);

	//	Perform the splitting
	RasterImage* colorChannelImage = splitImage(&image);

	// Produce the paths to the output files
	char** outFilePath = produceOutFilePath(inputImagePath, outFolderPath);

	//	Write out the cropped image
	for (unsigned int k=0; k<NUM_OF_CHANNELS; k++)
	{
		writeTGA(outFilePath[k], colorChannelImage + k);
	}
	//	Cleanup allocations.  Again, this is not really needed, since the full
	//	partition will get cleared when the process terminates, but I like to
	//	keep the good habit of freeing memory that I don't need anymore, and,
	//	if I crash, it's a sign that something went wrong earlier and I may
	//	have produced junk
	for (unsigned k=0; k<NUM_OF_CHANNELS; k++)
	{
		free(outFilePath[k]);
		deleteRasterImage(colorChannelImage+k);
	}
	free(outFilePath);
	deleteRasterImage(&image);
	
	return 0;
}

RasterImage* splitImage(RasterImage* img)
{
	RasterImage* channels = (RasterImage*) calloc(NUM_OF_CHANNELS, sizeof(RasterImage));
	for (unsigned int k=0; k<NUM_OF_CHANNELS; k++)
		channels[k] = newRasterImage(img->numRows, img->numCols, RGBA32_RASTER);
	
	int** rgbaIn = (int**) (img->raster2D);
	int** red = (int**) (channels[RED_CHANNEL].raster2D);
	int** green = (int**) (channels[GREEN_CHANNEL].raster2D);
	int** blue = (int**) (channels[BLUE_CHANNEL].raster2D);
	for (unsigned int i=0; i<img->numRows; i++)
	{
		for (unsigned int j=0; j<img->numCols; j++)
		{
			int pixel = rgbaIn[i][j];
			
			//	extract r, g, b chanels
			red[i][j] = pixel & 0xFF0000FF;
			green[i][j] = pixel & 0xFF00FF00;
			blue[i][j] = pixel & 0xFFFF0000;
		}
	}
	return channels;
}


char** produceOutFilePath(char* inputImagePath, char* outFolderPath)
{
	// Produce the name of the output files
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

	char** outFilePath = (char**) calloc(NUM_OF_CHANNELS, sizeof(char*));
	for (unsigned int k=0; k<NUM_OF_CHANNELS; k++)
	{
		outFilePath[k] = (char*) malloc(strlen(outFolderPath) +
										strlen(inputFileRootName) + strlen("-r.tga") + 2);
		strcpy(outFilePath[k], outFolderPath);
		//	If outFolderPath didn't end with a slash, add it
		if (outFolderPath[strlen(outFolderPath)-1] != '/')
			strcat(outFilePath[k], "/");
		strcat(outFilePath[k], inputFileRootName);
	}
	//	Append root name to output path, add "_r|g|b.tga"
	strcat(outFilePath[RED_CHANNEL], "_r.tga");
	strcat(outFilePath[GREEN_CHANNEL], "_g.tga");
	strcat(outFilePath[BLUE_CHANNEL], "_b.tga");

	//	free heap-allocated data we don't need anymore
	free(inputFileRootName);
	
	return outFilePath;
}

void errorReport(ErrorCode code, const char* input)
{
	if (code != NO_ERROR)
	{
		switch (code)
		{
			case WRONG_NUMBER_OF_ARGUMENTS:
			break;
			
			default:
				break;
		}
		exit(code);
	}
}



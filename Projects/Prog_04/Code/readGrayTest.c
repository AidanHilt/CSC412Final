//
//  split.c
//  Prog 02 Solution
//
//  Created by Jean-Yves Hervé on 2018-11.21.
//	Revised 2019-10-18
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//
#include "imageIO_TGA.h"

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
void errorReport(ErrorCode code, char* input);

/**	Produces a complete path to the each of the output image files.
 *	If the input file path was ../../Images/cells.tga and the
 *	and the output folder path is ../Output [with or without final slash),
 *	then one output file path will be ../Output/cells [orig].tga
 *	and the other will be ../Output/cells [copy].tga
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

	//----------------------------------------------
	//	Image read from a file
	//----------------------------------------------
	char* inputImagePath = argv[1];
	char* outputFolderPath = argv[2];
	
	//	Read the image
	RasterImage image = readTGA(inputImagePath);
	RasterImage copyImg = copyRasterImage(&image);
	char** outFilePath = produceOutFilePath(inputImagePath, outputFolderPath);

	writeTGA(outFilePath[0], &image);
	writeTGA(outFilePath[1], &copyImg);

	//----------------------------------------------
	//	An image with generated/computed values
	//----------------------------------------------
	RasterImage generatedImage = newRasterImage(256, 100, GRAY_RASTER);
	unsigned char** gray2D = (unsigned char**)(generatedImage.raster2D);
	//	Fill in values for the generated image
	for (unsigned int i=0; i<256; i++)
	{
		for (unsigned int j=0; j<100; j++)
		{
			gray2D[i][j] = (unsigned char) ((255-i)*(100-j)/100.0);
		}
	}
	
	char* genFilePath = (char*) malloc(strlen(outputFolderPath) +
									   strlen("generated.tga") + 2);
	strcpy(genFilePath, outputFolderPath);
	//	If outFolderPath didn't end with a slash, add it
	if (outputFolderPath[strlen(outputFolderPath)-1] != '/')
		strcat(genFilePath, "/");
	strcat(genFilePath, "generated.tga");
	writeTGA(genFilePath, &generatedImage);

	printf("Done.\n");
	
	//	unnecessary cleanup, but if I crash, it's a sign that something went wrong earlier
	//	and I may have produced junk
	free(outFilePath[0]);
	free(outFilePath[1]);
	free(outFilePath);
	free(genFilePath);
	deleteRasterImage(&image);
	deleteRasterImage(&copyImg);
	deleteRasterImage(&generatedImage);
	
	return 0;
}


void errorReport(ErrorCode code, char* input)
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

char* appendExtension(char* outFolderPath, char* inputFileRootName, char* extension)
{
	char* outFilePath = (char*) malloc(strlen(outFolderPath) +
									strlen(inputFileRootName) + strlen("-r.tga") + 2);
	strcpy(outFilePath, outFolderPath);
	//	If outFolderPath didn't end with a slash, add it
	if (outFolderPath[strlen(outFolderPath)-1] != '/')
		strcat(outFilePath, "/");

	strcat(outFilePath, inputFileRootName);
	//	Append root name to output path, add "_r|g|b.tga"
	strcat(outFilePath, extension);

	return outFilePath;
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

	char** outFilePath = (char**) calloc(2, sizeof(char*));
	outFilePath[0] = appendExtension(outFolderPath, inputFileRootName, " [orig].tga");
	outFilePath[1] = appendExtension(outFolderPath, inputFileRootName, " [copy].tga");

	//	free heap-allocated data we don't need anymore
	free(inputFileRootName);
	
	return outFilePath;
}

//
//  rotate
//  Prog 02 Solution
//
//  Created by Jean-Yves Hervé on 2018-10-03
//	Revised 2019-10-17
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

/**	Simple enum type to report optional arguments for the program
 */
typedef enum RotationVal
{
	NO_ROTATION = 0,
	ROTATE_90,
	ROTATE_180,
	ROTATE_270,
	//
	NUM_ROTATIONS	//	automatically 4
	
} RotationVal;

/**	An array to store the suffix strings for the output file, for each
 *	rotation value.
 */
char* ROT_SUFFIX[NUM_ROTATIONS] = {
	"",
	" [r]",
	" [rr]",
	" [l]"
};

/**	An enum type for all the errors that this program specifically handles
 */
typedef enum ErrorCode
{
	NO_ERROR = 0,
	//
//	//	file-related	-->	Not used in this application
//	FILE_NOT_FOUND,
//	CANNOT_OPEN_FILE,
//	WRONG_FILE_TYPE,
//	CANNOT_WRITE_FILE,
	//
	//	command line argument errors
	WRONG_NUMBER_OF_ARGUMENTS,
	NO_DASH_ON_ROT_STRING,
	INVALID_ROT_STRING,
	//
	NUM_ERROR_CODES		//	correct value because I don't skip codes
	
} ErrorCode;

/**	Going overly cute here:  Error message for each of the errors
 *	supported.  This one is tricky because it's easy to get the order
 *	different from that of the enum type.  In C++ enum types, we could
 *	directly associate a string to each enum value.
 */
char* ERROR_STR[NUM_ERROR_CODES] = {
	"",		//	NO_ERROR
//	//	file-related errors
//	"",	//	FILE_NOT_FOUND,
//	"",	//	CANNOT_OPEN_FILE,
//	"",	//	WRONG_FILE_TYPE,
//	"",	//	CANNOT_WRITE_FILE,
	//	command line argument errors
	"Incorrect number of arguments.\nProper usage: rotate -{r|l}+ inputImagePath outFolderPath\n",	//	WRONG_NUMBER_OF_ARGUMENTS
	"Rotation specification must start with a dash",	//	NO_DASH_ON_ROT_STRING,
	"Rotation specification can only contain letters l or r"	//	INVALID_ROT_STRING,
};

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Function prototypes
//-------------------------------------------------------------------
#endif

/**	Processes the rotation-specifying string to determine the
 *	rotation to apply.
 *	@param	rotStr	the rotation-specifying string in the form -[r|l]+
 *	@param	rotVal	pointer to a RotationVal enum variable
 *	@return	an error code
 */
ErrorCode determineRotation(char* rotStr, RotationVal* rotVal);

/**	In this app, just prints out an error message to the console and
 *	exits with the proper error code.  In a fancier version, could
 *	write to a log and "swallow" some non-critical errors.
 *
 *	@param code		the code of the error to report/process
 *	@param input	the input string that caused the error (NULL otherwise)
 */
void errorReport(ErrorCode code, char* input);

/**	Produces a complete path to the output image file.
 *	If the input file path was ../../Images/clown and the 90-degree-rotated is to be
 *	written the output folder path is ../Output [with or without final slash),
 *	then the output file path will be ../Output/clown [r].tga
 *
 *	@param inputImagePath	path to the input image
 *	@param outFolderPath	path to the output folder
 *	@param rotVal			the rotation applied
 *	@return	complete path to the desired output file.
 */
char* produceOutFilePath(char* inputImagePath, char* outFolderPath,
						 RotationVal rotVal);


/**	Produces a rotated copy of the input image (rotated by 90 degree clockwise).
 *	This function currently only works for RGBA32_RASTER images.
 *	@param	image	pointer to the RGBA32_RASTER image to rotate
 *	@return	a new image struct that stores the rotated image
 */
RasterImage rotateImage90(RasterImage* image);

/**	Produces a rotated copy of the input image (rotated by 180 degree clockwise).
 *	This function currently only works for RGBA32_RASTER images.
 *	@param	image	pointer to the RGBA32_RASTER image to rotate
 *	@return	a new image struct that stores the rotated image
 */
RasterImage rotateImage180(RasterImage* image);

/**	Produces a rotated copy of the input image (rotated by 270 degree clockwise).
 *	This function currently only works for RGBA32_RASTER images.
 *	@param	image	pointer to the RGBA32_RASTER image to rotate
 *	@return	a new image struct that stores the rotated image
 */
RasterImage rotateImage270(RasterImage* image);


#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Function implementations
//-------------------------------------------------------------------
#endif

//--------------------------------------------------------------
//	Main function, expecting as arguments:
//		  {rotation} inputImagePath outFolderPath
//	e.g.	l ../Images/cells.tga ../Output
//	It returns an error code (0 for no error)
//--------------------------------------------------------------
int main(int argc, char* argv[])
{
	//	we need at least one argument
	if (argc != 4)
		errorReport(WRONG_NUMBER_OF_ARGUMENTS, NULL);
	
	//	Just to look prettier in the code, I give meaningful names to my arguments
	char* inputImagePath = argv[2];
	char* outFolderPath = argv[3];

	//	Interpret the rotation argument
	RotationVal rot;
	int err = determineRotation(argv[1], &rot);
	if (err)
		errorReport(err, argv[1]);
	
	//	Read the image
	RasterImage image = readTGA(inputImagePath);
	
	//	Produce the path to the output image
	char* outFilePath = produceOutFilePath(inputImagePath, outFolderPath, rot);
	
	//	The way my newImage function is set up (it returns an RasterImage as value),
	//	I cannot separate declaration and initialization of my rotated image.
	//	It would be different if I was returning a pointer to a heap-allocated struct.
	//	That would get closer to "object-oriented C" in the sense that it would get
	//	fairly close to the behavior of
	//		Java:	ImageClass newImage;	//	reference to ImageClass object
	//				...
	//				newImage = new ImageClass(...);
	//
	//		C++:	ImageClas* newImage;	//	pointer to ImageClass object
	//				...
	//				newImage = new ImageClass(...);
	//
	//	Here, I have no choice but to have slighly different copies of basically the
	//	same code within the cases of my sitch statement
	//	The code for each case must be between braces because I declare variables within.
	switch(rot)
	{
		case NO_ROTATION:
		{
			RasterImage outImage = copyRasterImage(&image);
			writeTGA(outFilePath, &outImage);
			deleteRasterImage(&outImage);
		}
		break;
		
		case ROTATE_90:
		{
			RasterImage outImage = rotateImage90(&image);
			writeTGA(outFilePath, &outImage);
			deleteRasterImage(&outImage);
		}
		break;
		
		case ROTATE_180:
		{
			RasterImage outImage = rotateImage180(&image);
			writeTGA(outFilePath, &outImage);
			deleteRasterImage(&outImage);
		}
		break;
		
		case ROTATE_270:
		{
			RasterImage outImage = rotateImage270(&image);
			writeTGA(outFilePath, &outImage);
			deleteRasterImage(&outImage);
		}
		break;
		
		//	do shut warnings up
		default:
			break;
	}
	
	//	Cleanup allocations.  Again, this is not really needed, since the full
	//	partition will get cleared when the process terminates, but I like to
	//	keep the good habit of freeing memory that I don't need anymore, and,
	//	if I crash, it's a sign that something went wrong earlier and I may
	//	have produced junk
	deleteRasterImage(&image);
	free(outFilePath);
	
	return 0;
}

ErrorCode determineRotation(char* rotStr, RotationVal* rotVal)
{
	//	string should start with a dash
	if (rotStr[0] != '-')
		return NO_DASH_ON_ROT_STRING;
	
	//	string should have characters besides the dash
	if (strlen(rotStr) < 2)
		return INVALID_ROT_STRING;
	
	*rotVal = NO_ROTATION;
	//	Iterate through the string, keep the count of poisitive and
	//	negative rotations
	for (unsigned int k=1; k<strlen(rotStr); k++)
	{
		switch (rotStr[k])
		{
			case 'r':
			case 'R':
			*rotVal = (*rotVal + 1) % NUM_ROTATIONS;
			break;
			
			case 'l':
			case 'L':
			*rotVal = (*rotVal + NUM_ROTATIONS - 1) % NUM_ROTATIONS;
			break;
			
			//	anything else is an error
			default:
				return INVALID_ROT_STRING;
				
		}
	}
	
	return NO_ERROR;
}


void errorReport(ErrorCode code, char* input)
{
	if (input != NULL)
		printf("%s: %s\n", ERROR_STR[code], input);
	else
		printf("%s\n", ERROR_STR[code]);
	exit(code);
}


char* produceOutFilePath(char* inputImagePath, char* outFolderPath,
						 RotationVal rotVal)
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

	//	The +5 below is for the length of ".tga" plus the final string-terminating \0
	char* outFilePath = (char*) malloc(strlen(outFolderPath) +
									    strlen(inputFileRootName) +
									    strlen(ROT_SUFFIX[rotVal]) + 5);
	strcpy(outFilePath, outFolderPath);
	//	If outFolderPath didn't end with a slash, add it
	if (outFolderPath[strlen(outFolderPath)-1] != '/')
		strcat(outFilePath, "/");

	//	Append root name to output path, add the suffix and the file extension
	strcat(outFilePath, inputFileRootName);
	strcat(outFilePath, ROT_SUFFIX[rotVal]);
	strcat(outFilePath, ".tga");

	//	free heap-allocated data we don't need anymore
	free(inputFileRootName);
	
	return outFilePath;
}


//	In a rotation by 90 degree clockwise, the pixel at row i, col j in the input image
//	ends up at row outHeight - j - 1, col i in the output image
RasterImage rotateImage90(RasterImage* image)
{
	RasterImage outImage = newRasterImage(image->numCols, image->numRows, RGBA32_RASTER);
	int* rasterIn = (int*)(image->raster);
	int* rasterOut = (int*)(outImage.raster);

	for (unsigned i=0; i<image->numRows; i++)
	{
		for (unsigned j=0; j<image->numCols; j++)
			rasterOut[(outImage.numRows-j-1)*outImage.numCols + i] =
			rasterIn[i*image->numCols + j];
	}

	return outImage;
}

//	In a rotation by 180 degree clockwise, the pixel at row i, col j in the input image
//	ends up at row height - i - 1, col weight - j in the output image
RasterImage rotateImage180(RasterImage* image)
{
	RasterImage outImage = newRasterImage(image->numRows, image->numCols, RGBA32_RASTER);

	int* rasterIn = (int*)(image->raster);
	int* rasterOut = (int*)(outImage.raster);

	for (unsigned i=0; i<image->numRows; i++)
	{
		for (unsigned j=0; j<image->numCols; j++)
			rasterOut[(outImage.numRows-i-1)*outImage.numCols + outImage.numCols - j -1] =
			rasterIn[i*image->numCols + j];
	}

	return outImage;
}

//	In a rotation by 270 degree clockwise, the pixel at row i, col j in the input image
//	ends up at row j, col outWidth - i - 1 in the output image
RasterImage rotateImage270(RasterImage* image)
{
	RasterImage outImage = newRasterImage(image->numCols, image->numRows, RGBA32_RASTER);
	int* rasterIn = (int*)(image->raster);
	int* rasterOut = (int*)(outImage.raster);

	for (unsigned i=0; i<image->numRows; i++)
	{
		for (unsigned j=0; j<image->numCols; j++)
			rasterOut[j*outImage.numCols + outImage.numCols - i - 1] =
			rasterIn[i*image->numCols + j];
	}

	return outImage;
}

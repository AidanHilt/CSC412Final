 /*----------------------------------------------------------------------------------+
|	This is a modified version of the so-called "Lighthouse Library" for reading	|
|	images encoded in the *uncompressed, uncommented .tga (TARGA file) format. 		|
|	I had been using and modifying this code for years, simply mentioning			|
|	"Source Unknown" in my comments when I finally discovered, thanks to Web		|
|	searches, the origin of this code.  By then it had been adapted to work along	|
|	with reader/writer code for another image file format: the awful PPM/PBM/PGM	|
|	format of obvious Unix origin.													|
|	This is just to say that I am not claiming authorship of this code.  I am only	|
|	distributing it in this form instead of the original because (1) I have long	|
|	lost the original code, and (2) This version works with images organized		|
|	nicely into a struct.															|
|																					|
|	Jean-Yves Hervé		Dept. of Computer Science and Statistics, URI				|
|						2019-10-03													|
+----------------------------------------------------------------------------------*/


#include <stdlib.h>        
#include <stdio.h>
#include <dirent.h>
#include <errno.h>

#include "imageIO_TGA.h"


void swapRGBA(unsigned char *theData, unsigned int nbRows, unsigned int nbCols);


//----------------------------------------------------------------------
//	Utility function for memory swapping
//	Used because TGA stores the RGB data in reverse order (BGR)
//----------------------------------------------------------------------
void swapRGB(unsigned char *theData, unsigned int nbRows, unsigned int nbCols)
{
	unsigned int imgSize = nbRows*nbCols;

	for(unsigned int k = 0; k < imgSize; k++)
	{
		unsigned char tmp = theData[k*3+2];
		theData[k*3+2] = theData[k*3];
		theData[k*3] = tmp;
	}
}

void swapRGBA(unsigned char *theData, unsigned int nbRows, unsigned int nbCols)
{
    unsigned int imgSize = nbRows*nbCols;
    
    for(unsigned int k=0; k<imgSize; k++){
        unsigned char temp = theData[k*4+2];
        theData[k*4+2] = theData[k*4];
        theData[k*4] = temp;
    }
}



// ---------------------------------------------------------------------
//	Function : readTGA 
//	Description :
//	
//	This function reads an image of type TGA (8 or 24 bits, uncompressed
//	
//	Parameters:
//
//	*fileName - Pointer to a string that contains the name of the file
//	*nbCols, *nbRows - Dimensions XxY of the image
//	
//	Return value: pointer to the pixel data
//----------------------------------------------------------------------

unsigned char* readTGA(const char* fileName, unsigned int* nbRows, unsigned int* nbCols, ImageType* theType)
{
	unsigned char *theData, *ptr;
	unsigned int imgSize;
	char	head[18] ;
	FILE	*tga_in;
	ImageFileType fileType;
	//Voiding out the fileType variable so gcc doesn't freak out and annoy me
	(void)fileType;

	/* --- open TARGA input file ---------------------------------- */
	tga_in = fopen(fileName, "rb" );

	if (tga_in == NULL)
	{
		char* OUTPUT[2] = {" ", "Cannot open image file\n"};
		reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
		exit(1);
	}

	/* --- Read the header (TARGA file) --- */
	fread( head, sizeof(char), 18, tga_in ) ;
	/* Get the size of the image */
	*nbCols = (((unsigned int)head[12]&0xFF) | (unsigned int)head[13]*256);
	*nbRows = (((unsigned int)head[14]&0xFF) | (unsigned int)head[15]*256);


	if((head[2] == 2) && (head[16] == 24))
		*theType = RGBA32_RASTER;
	else if((head[2] == 3) && (head[16] == 8))
		*theType = GRAY_RASTER;
	else
	{
		char* OUTPUT[8] = {" ", "Unsuported TGA image: ", "Its type is ", &head[2], " and it has ", &head[16],
		 "bits per pixel.\n", "The image must be uncompressed while having 8 or 24 bits per pixel.\n"};
		reportAndWriteError(OUTPUT, 8, LOGFILENAME, 1);
		fclose(tga_in);
		exit(2);
	}

	imgSize = *nbCols * *nbRows;
	/* Create the buffer for image */

	if (*theType == RGBA32_RASTER)
		theData = (unsigned char*) malloc(imgSize*4);
	else
		theData = (unsigned char*) malloc(imgSize);

	if(theData == NULL)
	{
		char* OUTPUT[2] = {" ", "Unable to allocate memory\n"};
		reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
		fclose(tga_in);
		exit(3);
	}

	/* Check if the image is vertically mirrored */
	if (*theType == RGBA32_RASTER)
	{
		if(head[17]&0x20)
		{
			ptr = theData + imgSize*3 - ((*nbCols)*3);  
			for(unsigned int i = 0; i < *nbRows; i++)
			{
				fread( ptr, 3*sizeof(char), *nbCols, tga_in ) ;
				ptr -= (*nbCols)*3;
			}
		}
		else
        {
            unsigned char* dest = theData;
            for (unsigned int i=0; i<*nbRows; i++)
            {
                for (unsigned int j=0; j<*nbCols; j++)
                {
                    fread(dest, 3*sizeof(char), 1, tga_in);
                    dest+=4;
                }
            }
			
        }
        
        //  tga files store color information in the order B-G-R
        //  we need to swap the Red and Blue components
    	swapRGBA(theData, *nbRows, *nbCols);
	}
	else
	{
		if(head[17]&0x20)
		{
			ptr = theData + imgSize - *nbCols;  
			for(unsigned int i = 0; i < *nbRows; i++)
			{
				fread( ptr, sizeof(char), *nbCols, tga_in ) ;
				ptr -= *nbCols;
			}
		}
		else
		fread(theData, sizeof(char), imgSize, tga_in);
	}

	fclose( tga_in) ;
	return(theData);
}	


//---------------------------------------------------------------------*
//	Function : writeTGA 
//	Description :
//	
//	 This function write out an image of type TGA (24-bit color)
//	
//	 Parameters:
//	
//	  *fileName - Pointer to a string that contains the name of the file
//	  nbCols, nbRows - Dimensions XxY of the image
//	  *data - pointer to the array containing the pixels. This array
//	          is of type char {r, g, b, r, g, b, ... }
//
//	Return value: Error code (0 = no error)
//----------------------------------------------------------------------*/ 
int writeTGA(char* fileName, unsigned char* theData, unsigned int nbRows, unsigned int nbCols)
{
	long	offset;
	//int		swapflag = 1;
	char	head[18] ;
	FILE	*tga_out;
  
	/* --- open TARGA output file ---------------------------------- */

	tga_out = fopen(fileName, "wb" );

	if ( !tga_out )
	{
		printf("Cannot create pixel file %s \n", fileName);
		return 1;
	}

	// --- create the header (TARGA file) ---
	head[0]  = 0 ;		  					// ID field length.
	head[1]  = 0 ;		  					// Color map type.
	head[2]  = 2 ;		  					// Image type: true color, uncompressed.
	head[3]  = head[4] = 0 ;  				// First color map entry.
	head[5]  = head[6] = 0 ;  				// Color map lenght.
	head[7]  = 0 ;		  					// Color map entry size.
	head[8]  = head[9] = 0 ;  				// Image X origin.
	head[10] = head[11] = 0 ; 				// Image Y origin.
	head[13] = (char) (nbCols >> 8) ;		// Image width.
	head[12] = (char) (nbCols & 0x0FF) ;
	head[15] = (char) (nbRows >> 8) ;		// Image height.
	head[14] = (char) (nbRows & 0x0FF) ;
	head[16] = 24 ;		 					// Bits per pixel.
	head[17] = 0 ;		  					// Image descriptor bits ;

	fwrite(head, sizeof(char), 18, tga_out);
	
	for(unsigned int i = 0; i < nbRows; i++)
	{
		offset = i*4*nbCols;
		for(unsigned int j = 0; j < nbCols; j++)
		{
			fwrite(&theData[offset+2], sizeof(char), 1, tga_out);
			fwrite(&theData[offset+1], sizeof(char), 1, tga_out);
			fwrite(&theData[offset], sizeof(char), 1, tga_out);
			offset+=4;
		}
	}

	fclose( tga_out );

	return 0;
}	

/**
 * FILE* input: A FILE* that points to a valid TGA file.
 * Given a FILE* to a valid TGA file, will read it into 
 * a RasterImage struct and then return it.
 */
RasterImage structureImageFromFile(const char* fileName){
    unsigned int numRows, numColumns;
    ImageType imageType;
    unsigned char* raster;
    unsigned int bytesPerPixel, bytesPerRow;

    raster = readTGA(fileName, &numRows, &numColumns, &imageType);
	char* OUTPUT[2] = {" ", "This is not an image of the type RGBA32_RASTER. Aborting\n"};

    if(raster != NULL){
        switch(imageType){
            case RGBA32_RASTER: 
                bytesPerPixel = 4;
                break;
            default:
				reportAndWriteError(OUTPUT, 2, LOGFILENAME, 1);
				free(raster);
                exit(-1);
        }

        bytesPerRow = bytesPerPixel * numColumns;

        RasterImage retVal = {raster, numColumns, numRows, bytesPerPixel, bytesPerRow};
        return retVal;
    }else{
        printf("The image file failed to read\n");
        exit(-2);
    }
}

/**
 * RasterImage image: The RasterImage whose heap-allocated 
 * memory is to be cleared
 */
void clearRasterImage(RasterImage image){
	free(image.raster);
}

/**
 * char** itemsToWrite: An array of strings that should be written to the log file
 * char* fileName: The name of the file to be opened and written to
 * int argCount: The number of items in itemsToWrite
 * Given the following arguments, appends the designated strings to the given log file, then
 * closes it again
 */
void writeLog(char** itemsToWrite, char* fileName, int argCount){
	FILE* file = fopen(fileName, "a");

	if(file != NULL){
		for(int i = 0; i < argCount; i++){
			fputs(itemsToWrite[i], file);
		}
	}

	fclose(file);
}

/**
 * char** itemsToWrite: An array of strings that should be written to the log file
 * char* fileName: The name of the file to be opened and written to
 * int argCount: The number of items in itemsToWrite
 * int offset: If there are items that should be in the log that shouldn't be printed (i.e. whitespace or newlines)
 * offset designates where those end, and printable characters begin
 * Given the following arguments, appends the designated strings to the given log file, then
 * closes it again. After that, starting from offset to the end of itemsToWrite, it prints out the items
 * to the printf
 */
void reportAndWriteError(char** itemsToWrite, int argCount, char* fileName, int offset){
	writeLog(itemsToWrite, fileName, argCount);

	for(int i = offset; i < argCount; i++){
		printf("%s", itemsToWrite[i]);
	}
}

/**
 * char* directory: The path to a supposed directory whose existence
 * we want to check
 * Given a char* input of representing a directory, attempts to open 
 * it. If it succeeds, we close it and return an integer > 0. If it 
 * fails, then we return an integer <= 0.
 */
int checkValidDirectory(char* directory){
    DIR *check = opendir(directory);
    if(check != NULL){
        return 1;
    }else{
        return 0;
    }
}


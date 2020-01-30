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
#include <string.h>
//
#include "imageIO_TGA.h"

//	Prototypes of "private" functions
void swapRGBA(unsigned char *imgRaster, unsigned int numRows, unsigned int numCols);


//----------------------------------------------------------------------
//	Utility function for memory swapping
//	Used because TGA stores the RGB data in reverse order (BGR)
//----------------------------------------------------------------------
void swapRGB(unsigned char *imgRaster, unsigned int numRows, unsigned int numCols)
{
	unsigned int imgSize = numRows*numCols;

	for(unsigned int k = 0; k < imgSize; k++)
	{
		unsigned char tmp = imgRaster[k*3+2];
		imgRaster[k*3+2] = imgRaster[k*3];
		imgRaster[k*3] = tmp;
	}
}

void swapRGBA(unsigned char *imgRaster, unsigned int numRows, unsigned int numCols)
{
    unsigned int imgSize = numRows*numCols;
    
    for(unsigned int k=0; k<imgSize; k++){
        unsigned char temp = imgRaster[k*4+2];
        imgRaster[k*4+2] = imgRaster[k*4];
        imgRaster[k*4] = temp;
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
//	*numCols, *numRows - Dimensions XxY of the image
//	
//	Return value: pointer to the pixel data
//----------------------------------------------------------------------

RasterImage readTGA(const char* fileName)
{
	char head[18];
	unsigned int numRows, numCols;
	ImageFileType fileType;
	ImageType imgType;

	/* --- open TARGA input file ---------------------------------- */
	FILE* tga_in = fopen(fileName, "rb" );

	if (tga_in == NULL)
	{
		printf("Cannot open image file\n");
		exit(41);
	}

	/* --- Read the header (TARGA file) --- */
	fread( head, sizeof(char), 18, tga_in ) ;
	/* Get the size of the image */
	numCols = (((unsigned int)head[12]&0xFF) | (unsigned int)head[13]*256);
	numRows = (((unsigned int)head[14]&0xFF) | (unsigned int)head[15]*256);

	if((head[2] == 2) && (head[16] == 24))
	{
		fileType = kTGA_COLOR;
		imgType = RGBA32_RASTER;
	}
	else if((head[2] == 3) && (head[16] == 8))
	{
		fileType = kTGA_GRAY;
		imgType = GRAY_RASTER;
	}
	else
	{
		printf("Unsuported TGA image: ");
		printf("Its type is %d and it has %d bits per pixel.\n", head[2], head[16]);
		printf("The image must be uncompressed while having 8 or 24 bits per pixel.\n");
		fclose(tga_in);
		exit(42);
	}

	//	Use our brand new "constructor"
	RasterImage img = newRasterImage(numRows, numCols, imgType);
	
	//	get the raster's base pointer
	unsigned char* imgRaster = (unsigned char*) (img.raster);

	if (fileType == kTGA_COLOR)
	{
		//	upside-down image file
		if(head[17]&0x20)
		{
			unsigned char* ptr;
			for(unsigned int i = 0; i < numRows; i++)
			{
				ptr = imgRaster + (numRows-i-1)*img.bytesPerRow;
                for (unsigned int j=0; j<numCols; j++)
                {
                    fread(ptr, 3*sizeof(char), 1, tga_in);
                    ptr += 4;
                }
			}
		}
		else
        {
			unsigned char* ptr = imgRaster;
            for (unsigned int i=0; i<numRows; i++)
            {
                for (unsigned int j=0; j<numCols; j++)
                {
                    fread(ptr, 3*sizeof(char), 1, tga_in);
                    ptr += 4;
                }
             }
        }
        
        //  tga files store color information in the order B-G-R
        //  we need to swap the Red and Blue components
    	swapRGBA(imgRaster, numRows, numCols);
	}
	else // fileType == kTGA_GRAY
	{
		//	upside-down image file
		if(head[17]&0x20)
		{
			unsigned char*ptr = imgRaster + (numRows-1)*img.bytesPerRow;
			for(unsigned int i = 0; i < numRows; i++)
			{
				fread( ptr, sizeof(char), numCols, tga_in ) ;
				ptr -= img.bytesPerRow;
			}
		}
		else
		{
			unsigned char* ptr = imgRaster;
            for (unsigned int i=0; i<numRows; i++)
            {
				fread( ptr, sizeof(char), numCols, tga_in ) ;
				ptr += img.bytesPerRow;
             }
		}
	}

	fclose( tga_in) ;
	return(img);
}	


//---------------------------------------------------------------------*
//	Function : writeTGA 
//	Description :
//	
//	 This function write out an image of type TGA (24-bit color or
//	 gray-level image)
//
//	Return value: Error code (0 = no error)
//----------------------------------------------------------------------*/ 
int writeTGA(char* filePath, RasterImage* img)
{
	char	head[18] ;
	FILE	*tga_out;
  
	/* --- open TARGA output file ---------------------------------- */

	tga_out = fopen(filePath, "wb" );

	if ( !tga_out )
	{
		printf("Cannot create pixel file %s \n", filePath);
		return 1;
	}

	// --- create the header (TARGA file) ---
	head[0]  = 0 ;		  					// ID field length.
	head[1]  = 0 ;		  					// Color map type.
    //  head[2] =????;  ------>  will be determined by the image's base raster type
	head[3]  = head[4] = 0 ;  				// First color map entry.
	head[5]  = head[6] = 0 ;  				// Color map length.
	head[7]  = 0 ;		  					// Color map entry size.
	head[8]  = head[9] = 0 ;  				// Image X origin.
	head[10] = head[11] = 0 ; 				// Image Y origin.
	head[13] = (char) (img->numCols >> 8) ;	// Image width.
	head[12] = (char) (img->numCols & 0x0FF);
	head[15] = (char) (img->numRows >> 8) ;	// Image height.
	head[14] = (char) (img->numRows & 0x0FF) ;
    //  head[16] = ????;  ------>  will be determined by the image's base raster type
	head[17] = 0 ;		  					// Image descriptor bits ;

	switch (img->type)
	{
		case RGBA32_RASTER:
		{
            head[2] = 2;  		  					// Image type: true color, uncompressed.
            head[16] = 24;  		 				// Bits per pixel.
            fwrite(head, sizeof(char), 18, tga_out);

            //	get the image's 2D rgba raster
            unsigned char** imgRaster = (unsigned char**) img->raster2D;
			for ( unsigned int i=0; i<img->numRows; i++)
				//  don't forget that tga is bgr
				for ( unsigned int j=0; j<img->bytesPerRow; j+=4)
				{
					fwrite(imgRaster[i]+j+2, sizeof(char), 1, tga_out);
					fwrite(imgRaster[i]+j+1, sizeof(char), 1, tga_out);
					fwrite(imgRaster[i]+j, sizeof(char), 1, tga_out);
				}

            fclose(tga_out);
		}
        break;
			
		case GRAY_RASTER:
		{
            head[2] = 3;  		  					// Image type: gray level.
            head[16] = 8;  		 					// Bits per pixel.
            fwrite(head, sizeof(char), 18, tga_out);

            //	get the image's 2D gray raster
            unsigned char** imgRaster = (unsigned char**) img->raster2D;
			for ( unsigned int i=0; i<img->numRows; i++)
				fwrite(imgRaster[i], sizeof(char), img->numCols, tga_out);

            fclose(tga_out);
		}
		break;
			
		default:
			printf("Error: This type of image (float?) cannot be written as a TGA file\n");
			exit(44);

	}
	
	fclose( tga_out );

	return 0;
}	


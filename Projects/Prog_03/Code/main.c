//====================================================================================
//	First attempt at simple image processing on the GPU, using OpenCL
//
//	This code is based on Apple's "Hello World" OpenCL code sample,
//	mixed with the "Lighthouse" library for loading/writing TGA image files
//
//	Authors:	Brett Saguid & Jean-Yves Hervé
//				©2015	3D Group for Interactive Visualization
//						Department of Computer Science and Statistics
//						University of Rhode Island
//====================================================================================


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
//
#include "ImageIO/imageIO_TGA.c"

//-------------------------------------------------------------------------------------
//	Definition of private data types
//-------------------------------------------------------------------------------------

//	This struct stores all relevant information about an image:  its dimensions and
//	number of bytes per pixel (8 or 24).
//	Note:  It took me years to discover that "num" is not a common abbreviation for "number"
//	on these shores (in French it is).  By then, there were too many "numSomething" throughout
//	my program to make it possible to ever remove them all.  If you are going to use
//	any of my code samples or handout, just get used to it.  Feel free to rename the
//	fields to something you prefer.
typedef	struct ImageInfoStruct {
	unsigned char* raster;
	int	numCols;
	int numRows;
	int bytesPerPixel;
	int bytesPerRow;
} ImageStruct;


//	Note that I completely hard-coded the file paths.  Obviously, you will
//	have to change that in your program.
#define IN_PATH		"../Images/bottles.tga"
#define OUT_PATH	"../Output/output.tga"

#define PIXEL_AS_4_BYTES_VERSION	1
#define	PIXEL_AS_1_INT_VERSION		2
//	Choose one version
#define VERSION		PIXEL_AS_4_BYTES_VERSION


int main(int argc, char** argv)
{
	unsigned int numRows, numCols;
	ImageType imgType;
	unsigned char bytesPerPixel;
	unsigned int bytesPerRow;
	
	unsigned char* raster = readTGA(IN_PATH, &numRows, &numCols, &imgType);
	
	if (raster != NULL)
	{
		switch (imgType)
		{
			case RGBA32_RASTER:
			case FLOAT_RASTER:
				bytesPerPixel = 4;
				break;
			
			case GRAY_RASTER:
				bytesPerPixel = 1;
			break;
		}

		bytesPerRow = bytesPerPixel * numCols;
	}
	else
	{
		printf("Reading of image file failed.\n");
		exit(-1);
	}


	#if VERSION == PIXEL_AS_1_INT_VERSION
		//	Version 2:
		//
		//	I view my raster as an array of pixels stored on a 4-byte int.
		//	I use boolean operators to kill the right channel
		int* pixelRaster = (int*) raster;
	
	#endif
	
	//	I do something simple with my image, before writing it back
	//	For example, I "kill" the green channel of the image, so that
	//	it will come out in magenta-ish tint.
	for (unsigned int i=0; i< numRows; i++)
	{
		for (unsigned int j=0; j< numCols; j++)
		{
			//	Here I do the same operation in two different ways, because this might
			//	give you some ideas on how to do your HDR calculations.  You can enable
			//	either version by setting the #define with the proper value to verify
			//	that they do the same thing.
			
			#if VERSION == PIXEL_AS_4_BYTES_VERSION

				//	Version 1:  I view my raster as an array of unsigned char
				//
				//	Even though the image on-file has 24 bytes per pixel, we store in
				//	memory as 4-bytes per pixel (because this aligns a pixel with the size of
				//	an int.  This is where the factor 4 comes from
				// the color channel order is red-green-blue-alpha, hence the +1
				raster[i*bytesPerRow + 4*j + 1] = 0x00;

			#elif VERSION == PIXEL_AS_1_INT_VERSION

				//	Version 2:
				//
				//	I view my raster as an array of pixels stored on a 4-byte int.
				//  and access my pixel.  Note the absence of 4 factor, thanks to pointer algebra.
				int* pixel = pixelRaster + i*numCols + j;
				//	I use boolean operators to kill the right channel.  Note that x86 CPUs use
				//	a little-endian ordering of int/float/etc, so I need to give my mask in
				//	reverse order of what I intend to do in memory.  I want RGBA --> R0BA
				//	set green to zero.  So my mask is written as follows:
				*pixel = (*pixel) & 0xFFFF00FF;
			
			#else
				#error invalid version setting
			#endif
		}
	}
	
	//	And we write back the modified image into the Output folder.
    if (writeTGA(OUT_PATH, raster, numRows, numCols))
	{
		printf("Writing out of the image failed.\n");
	}
	
    // Print a brief summary detailing the results
    //
    printf("Done!\n");
    	
    return 0;
}

//
//  RasterImage.c
//
//  Jean-Yves Herv [2019-10-15]
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//
#include "RasterImage.h"

//	In some cases, e.g. rendering of an image in OpenGL using the drawPixel function,
//	it is necessary that the rows of the image be a number of bytes multiple of
//	some word size (4/8/16).  Here I set 1, spo there is no padding.
#define	WORD_SIZE_ROW_PADDING	1

RasterImage newRasterImage(unsigned int numRows, unsigned int numCols, ImageType type){
	unsigned char bytesPerPixel;
	unsigned int bytesPerRow;
	switch (type)
	{
		case RGBA32_RASTER:
		bytesPerPixel = 4;
		break;
		
		case GRAY_RASTER:
		bytesPerPixel = 1;
		break;
		
//		case DEEP_GRAY_RASTER:
//		bytesPerPixel = 2;
//		break;
		
		case FLOAT_RASTER:
		bytesPerPixel = sizeof(float);
		break;
		
		default:
			break;
	}

	//	This is where I take care of the padding business
	bytesPerRow = ((bytesPerPixel * numCols + WORD_SIZE_ROW_PADDING - 1)/WORD_SIZE_ROW_PADDING)*WORD_SIZE_ROW_PADDING;
	unsigned int effectiveWidth = bytesPerRow / bytesPerPixel;
	void* raster = (void*) calloc(numRows*effectiveWidth, bytesPerPixel);
	void* raster2D;
	
	switch (type)
	{
		case RGBA32_RASTER:
		case GRAY_RASTER:
		{
			unsigned char* r1D = (unsigned char*) raster;
			unsigned char** r2D = (unsigned char**) calloc(numRows, sizeof(unsigned char*));
			raster2D = (void*) r2D;
			for (unsigned int i=0; i<numRows; i++)
				r2D[i] = r1D + i*bytesPerRow;
		}
		break;
		
//		case DEEP_GRAY_RASTER:
//		{
//			unsigned short* r1D = (unsigned short*) raster;
//			unsigned short** r2D = (unsigned short**) calloc(numRows, sizeof(unsigned short*));
//			raster2D = (void*) r2D;
//			for (unsigned int i=0; i<numRows; i++)
//				r2D[i] = r1D + i*effectiveWidth;
//		}
		break;
		
		case FLOAT_RASTER:
		{
			float* r1D = (float*) raster;
			float** r2D = (float**) calloc(numRows, sizeof(float*));
			raster2D = (void*) r2D;
			for (unsigned int i=0; i<numRows; i++)
				r2D[i] = r1D + i*effectiveWidth;
		}
		break;

		default:
			break;
	}

	RasterImage img = {
						numRows,
						numCols,
						type,
						bytesPerPixel,
						bytesPerRow,
						raster,
						raster2D
					};
	return img;
}

void deleteRasterImage(RasterImage* img)
{
	switch (img->type) {
		case RGBA32_RASTER:
		case GRAY_RASTER:
			free((unsigned char*)(img->raster));
			free((unsigned char**)(img->raster2D));
			break;
		
//		case DEEP_GRAY_RASTER:
//			free((unsigned short*)(img->raster));
//			free((unsigned short**)raster2D);
//			break;
		
		case FLOAT_RASTER:
			free((float*)(img->raster));
			free((float**)(img->raster2D));
			break;
		
		default:
			break;
	}

	img->raster = img->raster2D = NULL;
}


RasterImage copyRasterImage(RasterImage* image)
{
	RasterImage outImage = newRasterImage(image->numRows, image->numCols, image->type);
	memcpy( (unsigned char*) outImage.raster,
			(char*) image->raster,
			image->numRows * image->bytesPerRow);
	
	return outImage;
}

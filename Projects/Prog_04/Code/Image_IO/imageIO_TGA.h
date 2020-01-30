#ifndef	IMAGE_IO_TGA_H
#define	IMAGE_IO_TGA_H

#include "RasterImage.h"

RasterImage readTGA(const char* fileName);
int writeTGA(char* fileName, RasterImage* img);

#endif	//	IMAGE_IO_TGA_H

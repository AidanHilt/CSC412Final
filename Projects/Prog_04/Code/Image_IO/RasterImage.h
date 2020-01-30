#ifndef	RASTER_IMAGE_H
#define	RASTER_IMAGE_H

typedef enum ImageFileType
{
		kUnknownType = -1,
		kTGA_COLOR,				//	24-bit color image
		kTGA_GRAY,
		kPPM,					//	24-bit color image
		kPGM					//	8-bit gray-level image
} ImageFileType;

/**	This is the enum type that refers to images loaded in memory, whether
 *	they were read from a file, played from a movie, captured from a live
 *	video stream, or the result of calculations.
 *	Feel free to edit and add types you need for your project.
 */
typedef enum ImageType
{
		/**	Color image with 4 bytes per pixel
		 */
		RGBA32_RASTER,

		/**	Gray image with 1 byte per pixel
		 */
		GRAY_RASTER,

		/**	Monochrome image (either gray or one color channel of a color image)
		 *	stored in a float raster
		 */
		FLOAT_RASTER
			
} ImageType;


/**	This struct stores all information relative to an image
 */
typedef struct RasterImage
{
	/**	height of the image
	 */
	unsigned int numRows;
	
	/**	width of the image
	 */
	unsigned int numCols;
	
	/**	image type
	 */
	ImageType type;
	
	/** 1 or 4 bytes per pixel.  This is somewhat redundant with the
	 *	previous field.
	 */
	unsigned char bytesPerPixel;

	/** number of bytes on an image row: numCols*bytesPerPixel, possibly with padding
	 */
	unsigned int bytesPerRow;

	/** 1D raster.  I make it a void* to accommodate the case of float rasters.
	 */
	void* raster;

	/** 2D raster, scaffolded on the 1D raster.  Also a void*, to be cast to proper
	 *	when used.
	 */
	void* raster2D;

} RasterImage;

/**	Creates (and allocates) an image with the type and dimensions specified
 *	@param numRows	height of the image
 *	@param numCols	width of the image
 *	@param type		grey level, color, or float
 */
RasterImage newRasterImage(unsigned int numRows, unsigned int numCols, ImageType type);


/**	Produces a copy of the input image (rotation by 0).
 *	This function currently only works for RGBA32_RASTER images.
 *	@param  image	pointer to the RGBA32_RASTER image to copy
 *	@return	a new image struct that stores a copy of the input image
 */
RasterImage copyRasterImage(RasterImage* image);


/**	Frees the space allocated for an image
 *	@param img	pointer to the image free
 */
void deleteRasterImage(RasterImage* img);

/**
 * FILE* input: A FILE* that points to a valid TGA file.
 * Given a FILE* to a valid TGA file, will read it into 
 * a RasterImage struct and then return it.
 */
RasterImage structureImageFromFile(const char* fileName);


#endif	//	RASTER_IMAGE_H

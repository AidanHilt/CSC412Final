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


/**	This is the data type that you need to fill in
 */
typedef struct RasterImage
{
	unsigned char* raster;
	int numColumns;
	int numRows;
	int bytesPerPixel;
	int bytesPerRow;

} RasterImage;

#endif	//	RASTER_IMAGE_H

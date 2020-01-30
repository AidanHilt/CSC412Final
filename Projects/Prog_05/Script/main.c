 //
 //  rotate
 //  Prog 02 Solution
 //
 //  Created by Jean-Yves HERVE on 2018-10-03.
 //
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 //
 #if 0
 #pragma mark -
 #pragma mark Custom data types and global variables
 #endif
 /** Simple enum type to report optional arguments for the program
  */
 typedef enum RotationVal
 {
     NO_ROTATION = 0,
     ROTATE_90,
     ROTATE_180,
     ROTATE_270,
     //
     NUM_ROTATIONS   //  automatically 4
     
 } RotationVal;
 /** An array to store the suffix strings for the output file, for each
  *  rotation value.
  */
 const char* ROT_SUFFIX[NUM_ROTATIONS] = {
     "",
     " [r]",
     " [rr]",
     " [l]"
 };
 /** An enum type for all the errors that this program specifically handles
  */
 typedef enum ImageIOErrorCode
 {
     NO_ERROR = 0,
     //
     //  file-related
     FILE_PATH_NULL,
     FILE_NOT_FOUND,
     CANNOT_OPEN_READ_FILE,
     CANNOT_OPEN_WRITE_FILE,
     WRONG_FILE_TYPE,
     UNKNOWN_FILE_TYPE,
     CANNOT_WRITE_FILE,
     CANNOT_READ_FILE,
     END_OF_FILE_ERROR,
     //
     //  Generic errors
     MEMORY_ALLOCATION_ERROR,
     //
     //  command line argument errors
     WRONG_NUMBER_OF_ARGUMENTS,
     NO_DASH_ON_ROT_STRING,
     INVALID_ROT_STRING,
     //
     //
     INVALID_CROP_X_TYPE,
     INVALID_CROP_Y_TYPE,
     INVALID_CROP_CORNER,
     INVALID_CROP_WIDTH_TYPE,
     INVALID_CROP_HEIGHT_TYPE,
     INVALID_CROP_SIZE,
     //
     NUM_ERROR_CODES     //  correct value because I don't skip codes
     
 } ImageIOErrorCode;
 /** Going overly cute here:  Error message for each of the errors
  *  supported.  This one is tricky because it's easy to get the order
  *  different from that of the enum type
  */
 const char* ERROR_STR[NUM_ERROR_CODES] = {
     "",     //  NO_ERROR
     //
     //  file-related errors
     "File path is null",        //  FILE_PATH NULL
     "File not found",           //  FILE_NOT_FOUND,
     "Cannot open read file",    //  CANNOT_OPEN_READ_FILE,
     "Cannot open write file",   //  CANNOT_OPEN_WRITE_FILE,
     "Wrong file type",          //  WRONG_FILE_TYPE,
     "Unknown file type",        //  UNKNOWN_FILE_TYPE
     "Cannot write file",        //  CANNOT_WRITE_FILE,
     "Cannot read file",         //  CANNOT_READ_FILE,
     "End of file error",        //  END_OF_FILE_ERROR,
     //
     //  Generic errors
     "Memory allocation error",  //  MEMORY_ALLOCATION_ERROR
     //
     //  command line argument errors
     //
     "Incorrect number of arguments.\nProper usage: rotate -{r|l}+ inputImagePath outFolderPath\n",  //  WRONG_NUMBER_OF_ARGUMENTS
     "Rotation specification must start with a dash",    //  NO_DASH_ON_ROT_STRING,
     "Rotation specification can only contain letters l or r"    //  INVALID_ROT_STRING,
     //
     //
     "Invalid crop x",           //  INVALID_CROP_X_TYPE,
     "Invalid crop y",           //  INVALID_CROP_Y_TYPE,
     "Invalid crop corner",      //  INVALID_CROP_CORNER,
     "Invalid crop width",       //  INVALID_CROP_WIDTH_TYPE,
     "Invalid crop height",      //  INVALID_CROP_HEIGHT_TYPE,
     "Invalid crop size",        //  INVALID_CROP_SIZE,
     
 };
 /** This enumerated type is used by the image reading code.  You shouldn't have
  *  to touch this
  */
 typedef enum ImageFileType
 {
         kUnknownType = -1,
         kTGA_COLOR,             //  24-bit color image
         kTGA_GRAY,
         kPPM,                   //  24-bit color image
         kPGM                    //  8-bit gray-level image
 } ImageFileType;
 /** This is the enum type that refers to images loaded in memory, whether
  *  they were read from a file, played from a movie, captured from a live
  *  video stream, or the result of calculations.
  *  Feel free to edit and add types you need for your project.
  */
 typedef enum ImageType
 {
         /** No type, for an image that got freed (no more raster)
          */
         NO_RASTER,
          
         /** Color image with 4 bytes per pixel
          */
         RGBA32_RASTER,
         /** Gray image with 1 byte per pixel
          */
         GRAY_RASTER,
         /** Gray image with 2 bytes per pixel
          */
         DEEP_GRAY_RASTER,
         /** Monochrome image (either gray or one color channel of a color image)
          *  stored in a float raster
          */
         FLOAT_RASTER
             
 } ImageType;
 /** This is the data type to store all relevant information about an image.  After
  *  some thought, I have decided to store the 1D and 2D rasters as void* rather than
  *  having separate unsigned char and float pointers
  */
 typedef struct ImageStruct {
     /** Type of image stored
      */
     ImageType type;
     /** Maximum value for all fields of the image
      *  (only useful for DEEP_GRAY_RASTER and FLOAT_RASTER types)
      */
     unsigned short maxVal;
     /** Number of rows (height) of the image
      */
     unsigned int height;
     /** Number of columns (width) of the image
      */
     unsigned int width;
     /** Pixel depth
      */
     unsigned int bytesPerPixel;
     /** Number of bytes per row (which may be larger than
      *  bytesPerPixel * nbCols if rows are padded to a particular
      *  word length (e.g. multiple of 16 or 32))
      */
     unsigned int bytesPerRow;
     /** Pointer to the image data, cast to a void* pointer.  To
      *  access the data, you would have to cast the pointer to the
      *  proper type, e.g.
      *  <ul>
      *      <li><tt>(int*) raster</tt></li>
      *      <li><tt>(unsigned char*) raster</tt></li>
      *      <li><tt>(int*) raster</tt></li>
      *      <li><tt>(float*) raster</tt></li>
      *  </ul>
      */
     void* raster;
     /* Similarly here the 2D raster was cast to a void* pointer
      *  and would need to be cast back to the proper type to be used, e.g.
      *  <ul>
      *      <li><tt>(int**) raster</tt></li>
      *      <li><tt>(unsigned char**) raster</tt></li>
      *      <li><tt>(int**) raster</tt></li>
      *      <li><tt>(float**) raster</tt></li>
      *  </ul>
      */
     void* raster2D;
     
 } ImageStruct;
 #if 0
 #pragma mark -
 #pragma mark Function prototypes
 #endif
 //  Reads the header of a PPM/PGM file
 char getHeaderInfoPPM_PGM_(FILE* inPPM, const char* filePath,
                            ImageFileType* fileType,
                            unsigned int* nbRows, unsigned int* nbCols,
                            unsigned int* maxVal);
 /** No-frills function that reads an image file in the <b>uncompressed</b>, un-commented PPM/PGM 
  *  (<tt>.ppm</tt> or <tt>.pgm</tt>) file format. If the image cannot be read (file not found, 
  *  invalid format, etc.) the function simply terminates execution.
  *  @param  filePath    path to the file to read
  *  @return  a properly initialized ImageStruct storing the image read
  */
 ImageStruct readPPM_PGM(const char* filePath);
 /** Writes an image file in the PPM/PGM (<tt>.ppm</tt> or <tt>.pgm</tt>) file format.
  *  @param  filePath    path to the file to write
  *  @param  img     pointer to the ImageStruct of the image to write into a .ppm or .pgm file.
  *  @return error code indicating whether the image was read successfully or not.
  */
 ImageIOErrorCode writePPM_PGM(const char* filePath, ImageStruct* img);
 /** returns a properly initialized ImageStruct (including the 1D raster).
  *  @param  width   number of columns of the image
  *  @param  height  number of rows of the image
  *  @param  type    one of RGBA32_RASTER, GRAY_RASTER, or FLOAT_RASTER
  *  @param  wordSizeRowPadding word size at which rows should be rounded up.
  *          You should use a padding size of 4 for a GRAY_RASTER raster
  *          if you intend to use OpenGL to render the image.
  */
 ImageStruct newImage(unsigned int width, unsigned int height, ImageType type,
                      unsigned int wordSizeRowPadding);
 /** No-frills function that reads an image file in the <b>uncompressed</b>, un-commented TARGA 
  *  (<tt>.tga</tt>) file format. If the image cannot be read (file not found, invalid format, etc.)
  *  the function simply terminates execution.
  *  @param  filePath    path to the file to read
  *  @return  a properly initialized ImageStruct storing the image read
  */
 ImageStruct readTGA(const char* filePath);
 /** Writes an image file in the <b>uncompressed</b>, un-commented TARGA (<tt>.tga</tt>) file format.
  *  @param  filePath    path to the file to write
  *  @param  info        pointer to the ImageStruct of the image to write into a .tga file.
  *  @return 1 if the image was read successfully, 0 otherwise.
  */
 int writeTGA(char* filePath, ImageStruct* info);
 /** Processes the rotation-specifying string to determine the
  *  rotation to apply.
  *  @param  rotStr  the rotation-specifying string in the form -[r|l]+
  *  @param  rotVal  pointer to a RotationVal enum variable
  *  @return an error code
  */
 ImageIOErrorCode determineRotation(const char* rotStr, RotationVal* rotVal);
 /** In this app, just prints out an error message to the console and
  *  exits with the proper error code.  In a fancier version, could
  *  write to a log and "swallow" some non-critical errors.
  *
  *  @param code     the code of the error to report/process
  *  @param input    the input string that caused the error (NULL otherwise)
  */
 void errorReport(ImageIOErrorCode code, const char* input);
 /** Produces a complete path to the output image file.
  *  If the input file path was ../../Images/clown and the 90-degree-rotated is to be
  *  written the output folder path is ../Output [with or without final slash),
  *  then the output file path will be ../Output/clown [r].tga
  *
  *  @param inputImagePath   path to the input image
  *  @param outFolderPath    path to the output folder
  *  @param rotVal           the rotation applied
  *  @return complete path to the desired output file.
  */
 char* produceOutFilePath(const char* inputImagePath, const char* outFolderPath,
                          RotationVal rotVal);
 /** interprets the program's input argument to determine the crop region.
  *  @param  argv    list of input argument to the program
  *  @param  cropCornerX     x coordinate of crop region's corner
  *  @param  cropCornerY     y coordinate of crop region's corner
  *  @param  cropWidth       crop region's width
  *  @param  cropHeight      crop region's height
  *  @return     an error code (0 if no error)
  */
 int extractCropRegion(const char* argv[],
                       unsigned int imageWidth, unsigned int imageHeight,
                       unsigned int* cropCornerX, unsigned int* cropCornerY,
                       unsigned int* cropWidth, unsigned int* cropHeight);
 /** Produces a new image that is a cropped part of the input image
  *  @param  image           the image to crop
  *  @param  cropCornerX     x coordinate of the upper-left corner of the crop region
  *  @param  cropCornerY     y coordinate of the upper-left corner of the crop region
  *                          (counted from the top, so height - row index - 1
  *  @param  cropWidth       width of the crop region
  *  @param  cropHeight      height of the top region
  *  @return     the new image resulting from applying the crop
  */
 ImageStruct cropImage(ImageStruct *image,
                       unsigned int cropCornerX, unsigned int cropCornerY,
                       unsigned int cropWidth, unsigned int cropHeight);
 /** Produces a copy of the input image (rotation by 0).
  *  This function currently only works for RGBA32_RASTER images.
  *  @param  image   pointer to the RGBA32_RASTER image to copy
  *  @return a new image struct that stores a copy of the input image
  */
 ImageStruct copyImage(const ImageStruct* image);
 /** Produces a rotated copy of the input image (rotated by 90 degree clockwise).
  *  This function currently only works for RGBA32_RASTER images.
  *  @param  image   pointer to the RGBA32_RASTER image to rotate
  *  @return a new image struct that stores the rotated image
  */
 ImageStruct rotateImage90(const ImageStruct* image);
 /** Produces a rotated copy of the input image (rotated by 180 degree clockwise).
  *  This function currently only works for RGBA32_RASTER images.
  *  @param  image   pointer to the RGBA32_RASTER image to rotate
  *  @return a new image struct that stores the rotated image
  */
 ImageStruct rotateImage180(const ImageStruct* image);
 /** Produces a rotated copy of the input image (rotated by 270 degree clockwise).
  *  This function currently only works for RGBA32_RASTER images.
  *  @param  image   pointer to the RGBA32_RASTER image to rotate
  *  @return a new image struct that stores the rotated image
  */
 ImageStruct rotateImage270(const ImageStruct* image);
 #if 0
 #pragma mark -
 #pragma mark Function implementations
 #endif
 //--------------------------------------------------------------
 //  Main function, expecting as arguments:
 //        {rotation} inputImagePath outFolderPath
 //  e.g.    l ../Images/cells.tga ../Output
 //  It returns an error code (0 for no error)
 //--------------------------------------------------------------
 int main(int argc, const char* argv[])
 {
     //  we need at least one argument
     if (argc != 4)
         errorReport(WRONG_NUMBER_OF_ARGUMENTS, NULL);
     
     //  Just to look prettier in the code, I give meaningful names to my arguments
     const char* inputImagePath = argv[2];
     const char* outFolderPath = argv[3];
     //  Interpret the rotation argument
     RotationVal rot;
     int err = determineRotation(argv[1], &rot);
     if (err)
         errorReport(err, argv[1]);
     
     //  Read the image
     ImageStruct image = readTGA(inputImagePath);
     
     //  Produce the path to the output image
     char* outImagePath = produceOutFilePath(inputImagePath, outFolderPath, rot);
     
     //  The way my newImage function is set up (it returns an ImageStruct as value),
     //  I cannot separate declaration and initialization of my rotated image.
     //  It would be different if I was returning a pointer to a heap-allocated struct.
     //  That would get closer to "object-oriented C" in the sense that it would get
     //  fairly close to the behavior of
     //      Java:   ImageClass newImage;    //  reference to ImageClass object
     //              ...
     //              newImage = new ImageClass(...);
     //
     //      C++:    ImageClas* newImage;    //  pointer to ImageClass object
     //              ...
     //              newImage = new ImageClass(...);
     //
     //  Here, I have no choice but to have slighly different copies of basically the
     //  same code within the cases of my sitch statement
     //  The code for each case must be between braces because I declare variables within.
     switch(rot)
     {
         case NO_ROTATION:
         {
             ImageStruct outImage = copyImage(&image);
             writeTGA(outImagePath, &outImage);
             free(outImage.raster);
         }
         break;
         
         case ROTATE_90:
         {
             ImageStruct outImage = rotateImage90(&image);
             writeTGA(outImagePath, &outImage);
             free(outImage.raster);
         }
         break;
         
         case ROTATE_180:
         {
             ImageStruct outImage = rotateImage180(&image);
             writeTGA(outImagePath, &outImage);
             free(outImage.raster);
         }
         break;
         
         case ROTATE_270:
         {
             ImageStruct outImage = rotateImage270(&image);
             writeTGA(outImagePath, &outImage);
             free(outImage.raster);
         }
         break;
         
         //  do shut warnings up
         default:
             break;
     }
     
     //  Cleanup and terminate
     free(image.raster);
     free(outImagePath);
     
     return 0;
 }
 ImageIOErrorCode determineRotation(const char* rotStr, RotationVal* rotVal)
 {
     //  string should start with a dash
     if (rotStr[0] != '-')
         return NO_DASH_ON_ROT_STRING;
     
     //  string should have characters besides the dash
     if (strlen(rotStr) < 2)
         return INVALID_ROT_STRING;
     
     *rotVal = NO_ROTATION;
     //  Iterate through the string, keep the count of poisitive and
     //  negative rotations
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
             
             //  anything else is an error
             default:
                 return INVALID_ROT_STRING;
                 
         }
     }
     
     return NO_ERROR;
 }
 void errorReport(ImageIOErrorCode code, const char* input)
 {
     if (input != NULL)
         printf("%s: %s\n", ERROR_STR[code], input);
     else
         printf("%s\n", ERROR_STR[code]);
     exit(code);
 }
 char* produceOutFilePath(const char* inputImagePath, const char* outFolderPath,
                          RotationVal rotVal)
 {
     // Produce the name of the output file
     //-------------------------------------
     //  First, find the start of the input file's name.  Start from the end
     //  and move left until we hit the first slash or the left end of the string.
     unsigned long k = strlen(inputImagePath) - 5;
     while ((k>=1) && (inputImagePath[k-1] != '/'))
         k--;
     
     //  The +5 below is for the length of ".tga" plus the final string-terminating \0
     char* outFilePath = (char*) malloc(strlen(outFolderPath) +
                                        strlen(ROT_SUFFIX[rotVal]) + 5);
     strcpy(outFilePath, outFolderPath);
     //  If outFolderPath didn't end with a slash, add it
     if (outFolderPath[strlen(outFolderPath)-1] != '/')
         strcat(outFilePath, "/");
     //  Produce the name of the input file minus extension
     char* inputFileRootName = (char*) malloc(strlen(inputImagePath+k) +1);
     strcpy(inputFileRootName, inputImagePath+k);
     //  chop off the extension by replacing the dot by '\0'
     inputFileRootName[strlen(inputFileRootName)-4] = '\0';
     //  Append root name to output path, add the suffix and the file extension
     strcat(outFilePath, inputFileRootName);
     strcat(outFilePath, ROT_SUFFIX[rotVal]);
     strcat(outFilePath, ".tga");
     //  free heap-allocated data we don't need anymore
     free(inputFileRootName);
     
     return outFilePath;
 }
 ImageStruct copyImage(const ImageStruct* image)
 {
     ImageStruct outImage = newImage(image->width, image->height, RGBA32_RASTER, 4);
     memcpy( (char*) outImage.raster,
             (char*) image->raster,
             image->height * image->bytesPerRow);
     
     return outImage;
 }
 //  In a rotation by 90 degree clockwise, the pixel at row i, col j in the input image
 //  ends up at row outHeight - j - 1, col i in the output image
 ImageStruct rotateImage90(const ImageStruct* image)
 {
     ImageStruct outImage = newImage(image->height, image->width, RGBA32_RASTER, 4);
     const int* rasterIn = (int*)(image->raster);
     int* rasterOut = (int*)(outImage.raster);
     for (unsigned i=0; i<image->height; i++)
     {
         for (unsigned j=0; j<image->width; j++)
             rasterOut[(outImage.height-j-1)*outImage.width + i] =
             rasterIn[i*image->width + j];
     }
     return outImage;
 }
 //  In a rotation by 180 degree clockwise, the pixel at row i, col j in the input image
 //  ends up at row height - i - 1, col weight - j in the output image
 ImageStruct rotateImage180(const ImageStruct* image)
 {
     ImageStruct outImage = newImage(image->width, image->height, RGBA32_RASTER, 4);
     
     const int* rasterIn = (int*)(image->raster);
     int* rasterOut = (int*)(outImage.raster);
     for (unsigned i=0; i<image->height; i++)
     {
         for (unsigned j=0; j<image->width; j++)
             rasterOut[(outImage.height-i-1)*outImage.width + outImage.width - j -1] =
             rasterIn[i*image->width + j];
     }
     return outImage;
 }
 //  In a rotation by 270 degree clockwise, the pixel at row i, col j in the input image
 //  ends up at row j, col outWidth - i - 1 in the output image
 ImageStruct rotateImage270(const ImageStruct* image)
 {
     ImageStruct outImage = newImage(image->height, image->width, RGBA32_RASTER, 4);
     const int* rasterIn = (int*)(image->raster);
     int* rasterOut = (int*)(outImage.raster);
     for (unsigned i=0; i<image->height; i++)
     {
         for (unsigned j=0; j<image->width; j++)
             rasterOut[j*outImage.width + outImage.width - i - 1] =
             rasterIn[i*image->width + j];
     }
     return outImage;
 }
 void swapRGB(unsigned char* theData, int height, int width);
 void swapRGBA(unsigned char* theData, int height, int width);
 //----------------------------------------------------------------------
 //  Utility function for memory swapping
 //  Used because TGA stores the RGB data in reverse order (BGR)
 //----------------------------------------------------------------------
 void swapRGB(unsigned char* theData, int height, int width)
 {
     int         imgSize;
     unsigned char tmp;
     imgSize = height*width;
     for(int k = 0; k < imgSize; k++)
     {
         tmp = theData[k*3+2];
         theData[k*3+2] = theData[k*3];
         theData[k*3] = tmp;
     }
 }
 void swapRGBA(unsigned char* theData, int height, int width)
 {
     int imgSize;
     unsigned char temp;
     
     imgSize = height*width;
     
     for(int k=0; k<imgSize; k++){
         temp = theData[k*4+2];
         theData[k*4+2] = theData[k*4];
         theData[k*4] = temp;
     }
 }
 // ---------------------------------------------------------------------
 //  Function : readTGA 
 //  Description :
 //  
 //  This function reads an image of type TGA (8 or 24 bits, uncompressed
 //  
 //----------------------------------------------------------------------
 ImageStruct readTGA(const char* filePath)
 {
     ImageStruct info;
     //--------------------------------
     //  open TARGA input file
     //--------------------------------
     FILE* tga_in = fopen(filePath, "rb" );
     if (tga_in == NULL)
     {
         printf("Cannot open image file %s\n", filePath);
         exit(11);
     }
     //--------------------------------
     //  Read the header (TARGA file)
     //--------------------------------
     char    head[18] ;
     fread( head, sizeof(char), 18, tga_in ) ;
     /* Get the size of the image */
     info.width = (int)(((unsigned int)head[12]&0xFF) | (unsigned int)head[13]*256);
     info.height = (int)(((unsigned int)head[14]&0xFF) | (unsigned int)head[15]*256);
     unsigned int imgSize = info.height * info.width;
     unsigned char* data;
     if((head[2] == 2) && (head[16] == 24))
     {
         info.type = RGBA32_RASTER;
         info.bytesPerPixel = 4;
         info.bytesPerRow = 4*info.width;
         data = (unsigned char*) malloc(imgSize*4);
     }
     else if((head[2] == 3) && (head[16] == 8))
     {
         info.type = GRAY_RASTER;
         info.bytesPerPixel = 1;
         info.bytesPerRow = info.width;
         data = (unsigned char*) malloc(imgSize);
     }
     else
     {
         printf("Unsuported TGA image: ");
         printf("Its type is %d and it has %d bits per pixel.\n", head[2], head[16]);
         printf("The image must be uncompressed while having 8 or 24 bits per pixel.\n");
         fclose(tga_in);
         exit(12);
     }
     if(data == NULL)
     {
         printf("Unable to allocate memory\n");
         fclose(tga_in);
         exit(13);
     }
     info.raster = (void*) data;
     
     //--------------------------------
     //  Read the pixel data
     //--------------------------------
     //  Case of a color image
     //------------------------  
     //if (info.type == kTGA_COLOR) *****************************
     if(info.type == RGBA32_RASTER)
     {
         //  First check if the image is mirrored vertically (a bit setting in the header)
         if(head[17]&0x20)
         {
             unsigned char* ptr = data + imgSize*4 - info.bytesPerRow;
             for(int i = 0; i < info.height; i++)
             {
                 for (int j=0; j<info.width; j++)
                 {
                     fread(ptr, 3*sizeof(char), 1, tga_in);
                     ptr -= 4;
                 }
             }
         }
         else
         {
             unsigned char* dest = data;
             for (int i=0; i<info.height; i++)
             {
                 for (int j=0; j<info.width; j++)
                 {
                     fread(dest, 3*sizeof(char), 1, tga_in);
                     dest+=4;
                 }
             }
             
         }
         
         //  tga files store color information in the order B-G-R
         //  we need to swap the Red and Blue components
         swapRGBA(data, info.height, info.width);
     }
     //  Case of a gray-level image
     //----------------------------  
     else
     {
         //  First check if the image is mirrored vertically (a bit setting in the header)
         if(head[17]&0x20)
         {
             unsigned char* ptr = data + imgSize - info.width;
             for(int i = 0; i < info.height; i++)
             {
                 fread( ptr, sizeof(char), info.width, tga_in ) ;
                 ptr -= info.width;
             }
         }
         else
             fread(data, sizeof(char), imgSize, tga_in);
     }
     fclose( tga_in) ;
     return info;
 }   
 //---------------------------------------------------------------------*
 //  Function : writeTGA 
 //  Description :
 //  
 //   This function write out an image of type TGA (24-bit color)
 //  
 //  Return value: Error code (0 = no error)
 //----------------------------------------------------------------------*/ 
 int writeTGA(char* filePath, ImageStruct* info)
 {
     //--------------------------------
     // open TARGA output file 
     //--------------------------------
     FILE* tga_out = fopen(filePath, "wb" );
     if (tga_out == NULL)
     {
         printf("Cannot create image file %s \n", filePath);
         return 21;
     }
     //  Yes, I know that I tell you over and over that cascading if-else tests
     //  are bad style when testing an integral value, but here only two values
     //  are supported.  If I ever add one more I'll use a switch, I promise.
     
     //------------------------------
     //  Case of a color image
     //------------------------------
     if (info->type == RGBA32_RASTER)
     {
         //--------------------------------
         // create the header (TARGA file)
         //--------------------------------
         char    head[18] ;
         head[0]  = 0 ;                          // ID field length.
         head[1]  = 0 ;                          // Color map type.
         head[2]  = 2 ;                          // Image type: true color, uncompressed.
         head[3]  = head[4] = 0 ;                // First color map entry.
         head[5]  = head[6] = 0 ;                // Color map lenght.
         head[7]  = 0 ;                          // Color map entry size.
         head[8]  = head[9] = 0 ;                // Image X origin.
         head[10] = head[11] = 0 ;               // Image Y origin.
         head[13] = (char) (info->width >> 8) ;      // Image width.
         head[12] = (char) (info->width & 0x0FF) ;
         head[15] = (char) (info->height >> 8) ;     // Image height.
         head[14] = (char) (info->height & 0x0FF) ;
         head[16] = 24 ;                         // Bits per pixel.
         head[17] = 0 ;                          // Image descriptor bits ;
         fwrite( head, sizeof(char), 18, tga_out );
         unsigned char* data  = (unsigned char*) info->raster;
         for(int i = 0; i < info->height; i++)
         {
             unsigned long offset = i*4*info->width;
             for(int j = 0; j < info->width; j++)
             {
                 fwrite(&data[offset+2], sizeof(char), 1, tga_out);
                 fwrite(&data[offset+1], sizeof(char), 1, tga_out);
                 fwrite(&data[offset], sizeof(char), 1, tga_out);
                 offset+=4;
             }
         }
         fclose( tga_out ) ;
     }
     //------------------------------
     //  Case of a gray-level image
     //------------------------------
     else if (info->type == GRAY_RASTER)
     {
         //--------------------------------
         // create the header (TARGA file)
         //--------------------------------
         char    head[18] ;
         head[0]  = 0 ;                          // ID field length.
         head[1]  = 0 ;                          // Color map type.
         head[2]  = 3 ;                          // Image type: gray-level, uncompressed.
         head[3]  = head[4] = 0 ;                // First color map entry.
         head[5]  = head[6] = 0 ;                // Color map lenght.
         head[7]  = 0 ;                          // Color map entry size.
         head[8]  = head[9] = 0 ;                // Image X origin.
         head[10] = head[11] = 0 ;               // Image Y origin.
         head[13] = (char) (info->width >> 8) ;      // Image width.
         head[12] = (char) (info->width & 0x0FF) ;
         head[15] = (char) (info->height >> 8) ;     // Image height.
         head[14] = (char) (info->height & 0x0FF) ;
         head[16] = 8 ;                          // Bits per pixel.
         head[17] = 0 ;                          // Image descriptor bits ;
         fwrite( head, sizeof(char), 18, tga_out );
         unsigned char* data  = (unsigned char*) info->raster;
         for(int i = 0; i < info->height; i++)
         {
             fwrite(&data[i*info->width], sizeof(char), info->width, tga_out);
         }
         fclose( tga_out ) ;
     }
     else
     {
         printf("Image type not supported for output\n");
         return 22;
     }
     
     return 0;
 }   
 //---------------------------------------
 //  Image utility functions.
 //  Should be moved to some Image.c file
 //---------------------------------------
 ImageStruct newImage(unsigned int width, unsigned int height, ImageType type,
                      unsigned int wordSizeRowPadding)
 {
     if (width<3 || height<3)
     {
         printf("Image size should be at least 3x3\n");
         exit(14);
     }
     if (wordSizeRowPadding!=1 && wordSizeRowPadding!=4 &&
         wordSizeRowPadding!=8 && wordSizeRowPadding!=16 &&
         wordSizeRowPadding!=32 && wordSizeRowPadding!=64)
     {
         printf("wordSizeRowPadding must be one of: 1, 4, 8, 16, 32, or 64\n");
         exit(15);
     }
     ImageStruct img;
     img.width = width;
     img.height = height;
     img.type = type;
     switch (type)
     {
         case RGBA32_RASTER:
         img.bytesPerPixel = 4;
         break;
         
         case GRAY_RASTER:
         img.bytesPerPixel = 1;
         break;
         
         case FLOAT_RASTER:
         img.bytesPerPixel = sizeof(float);
         break;
         
         default:
             break;
     }
     img.bytesPerRow = ((img.bytesPerPixel * width + wordSizeRowPadding - 1)/wordSizeRowPadding)*wordSizeRowPadding;
     unsigned int effectiveWidth = img.bytesPerRow / img.bytesPerPixel;  
     img.raster = (void*) calloc(height*effectiveWidth, img.bytesPerPixel);
     return img;
 }
 int extractCropRegion(const char* argv[],
                       unsigned int imageWidth, unsigned int imageHeight,
                       unsigned int* cropCornerX, unsigned int* cropCornerY,
                       unsigned int* cropWidth, unsigned int* cropHeight)
 {
     if (sscanf(argv[3], "%u", cropCornerX) != 1)
         errorReport(INVALID_CROP_X_TYPE, argv[3]);
     if (sscanf(argv[4], "%u", cropCornerY) != 1)
         errorReport(INVALID_CROP_Y_TYPE, argv[3]);
     //  Note: since we read into an unsigned int, a negative value would come out
     //  as a large positive value
     if ((*cropCornerX >= imageWidth) || (*cropCornerY >= imageHeight))
         errorReport(INVALID_CROP_CORNER, NULL);
     if (sscanf(argv[5], "%u", cropWidth) != 1)
         errorReport(INVALID_CROP_WIDTH_TYPE, argv[3]);
     if (sscanf(argv[6], "%u", cropHeight) != 1)
         errorReport(INVALID_CROP_HEIGHT_TYPE, argv[3]);
     //  Note: since we read into an unsigned int, a negative value would come out
     //  as a large positive value
     if ((*cropCornerX + *cropWidth >= imageWidth) ||
         (*cropCornerY + *cropHeight >= imageHeight))
         errorReport(INVALID_CROP_SIZE, NULL);
     
     //  Otherwise, all is ok, go back to crop
     return 0;
 }
 ImageStruct cropImage(ImageStruct *imageIn,
                       unsigned int cropCornerX, unsigned int cropCornerY,
                       unsigned int cropWidth, unsigned int cropHeight)
 {
     ImageStruct imageOut = newImage(cropWidth, cropHeight, RGBA32_RASTER, 1);
     
     //  Beware that the images are stored upside-down from the way we view them,
     //  So I need to invert the row indices.
     for (unsigned int i = 0; i<cropHeight; i++)
     {
         memcpy((unsigned char*) imageOut.raster + (imageOut.height - i - 1)*imageOut.bytesPerRow,
                (unsigned char*) imageIn->raster + (imageIn->height - i - cropCornerY - 1)*imageIn->bytesPerRow
                                                 + cropCornerX * imageIn->bytesPerPixel,
                cropWidth*imageIn->bytesPerPixel);
         
     }
     return imageOut;
 }
 //----------------------------------------------------------------------
 //  readPPM()
 //  Goal :  read a 24-bit color image in PPM format (binary or ASCII)
 //  Input:  the image to read
 //          the input file name.
 //
 //  Rant:
 //      PPM (and its PBM/PGM brethens) is just about the stupidest image
 //      file format out there.  Only a Unix programmer could possibly have
 //      come up with something that dumb.  It is dumb because it mixes
 //      binary and ASCII stuff, and obliges you to work around the \r chars
 //      (if you are working on any other platform other than Unix, there *will*
 //      be \r characters), while Unix-centric stdio functions don't handle that
 //      character as an end of string.  Of course, all the code written by Unix
 //      geeks to read PPM will fail miserably if you try to run it on Windows
 //      or on a Mac because they simply ignore \r.
 //
 //----------------------------------------------------------------------
 ImageStruct readPPM_PGM(const char* filePath)
 {
     ImageStruct image;
     
     // --- open file ---
     FILE* inPPM = fopen(filePath, "r");
     if (inPPM == NULL)
     {
         printf("File %s not found\n", filePath);
         exit(FILE_NOT_FOUND);
     }
     //  reset dimensions (just to play it safe)
     unsigned int nbRows = 0, nbCols = 0, maxVal = 0;
     ImageFileType fileType;
     int isASCII = getHeaderInfoPPM_PGM_(inPPM, filePath, &fileType,
                                         &nbRows, &nbCols, &maxVal);
     
     if (fileType == kPPM)
     {
         //  Allocate the pixel array
         unsigned char* pixelData = (unsigned char*)calloc(4*nbCols*nbRows, 1);
         //  We only want to handle values in the [0, 255] range
         float valScale = (maxVal != 255) ?  255.f/maxVal : 1.f;
         //  The image is stored in an ASCII format
         if (isASCII)
         {
             for (unsigned int i = nbRows-1, invI = 0; invI < nbRows; i--, invI++)
             {
                 unsigned index = 4*i*nbCols;
                 
                 for (unsigned int j = 0; j < nbCols; j++)
                 {
                     int theRed, theGreen, theBlue;
                     fscanf(inPPM, "%d%d%d", &theRed, &theGreen, &theBlue);
                     pixelData[index++] = (unsigned char) (valScale*theRed);
                     pixelData[index++] = (unsigned char) (valScale*theGreen);
                     pixelData[index++] = (unsigned char) (valScale*theBlue);
                     pixelData[index++] = 0xFF;  //  alpha
                 }
             }
         }
         //  The image is stored in a binary format
         else
         {
             for (unsigned int i = nbRows-1, invI = 0; invI < nbRows; i--, invI++)
             {
                 unsigned index = 4*i*nbCols;
                 
                 for (unsigned j = 0; j < nbCols; j++)
                 {
                     char cRed, cGreen, cBlue;
                     fscanf(inPPM, "%c%c%c", &cRed, &cGreen, &cBlue);
                     //  for binary format I don't bother about the scaling
                     pixelData[index++] = (unsigned char) cRed;
                     pixelData[index++] = (unsigned char) cGreen;
                     pixelData[index++] = (unsigned char) cBlue;
                     pixelData[index++] = 0xFF;  //  alpha
                 }
             }
         }
         image.type = RGBA32_RASTER;
         image.bytesPerPixel = 4;
         image.bytesPerRow = 4*nbCols;
         image.raster = pixelData;
         //  Allocate raster2D
         unsigned char** r2D = (unsigned char**) calloc(nbRows, sizeof(unsigned char*));
         for (unsigned int i=0; i<nbRows; i++)
             r2D[i] = pixelData + i*image.bytesPerRow;
         image.raster2D = (void*) r2D;
     }
     else    //  kPGM
     {
         if (maxVal == 255)
         {
             //  Allocate the pixel array
             unsigned char* pixelData = (unsigned char*)calloc(nbCols*nbRows, 1);
             image.raster = pixelData;
             image.type = GRAY_RASTER;
             image.bytesPerPixel = 1;
             image.bytesPerRow = nbCols;
             //  Allocate raster2D
             unsigned char** r2D = (unsigned char**) calloc(nbRows, sizeof(unsigned char*));
             for (unsigned int i=0; i<nbRows; i++)
                 r2D[i] = pixelData + i*image.bytesPerRow;
             image.raster2D = (void*) r2D;
             //  The image is stored in an ASCII format
             if (isASCII)
             {
                 for (unsigned int i = nbRows-1, invI = 0; invI < nbRows; i--, invI++)
                 {
                     unsigned int index = i*nbCols;
                     
                     for (unsigned int j = 0; j < nbCols; j++)
                     {
                         int grayVal;
                         fscanf(inPPM, "%d", &grayVal);
                         pixelData[index++] = (unsigned char) (grayVal);
                     }
                 }
             }
             //  The image is stored in a binary format
             else
             {
                 for (unsigned int i = nbRows-1, invI = 0; invI < nbRows; i--, invI++)
                 {
                     unsigned int index = i*nbCols;
                     
                     for (unsigned j = 0; j < nbCols; j++, index++)
                     {
                         fscanf(inPPM, "%c", pixelData + index);
                     }
                 }
             }
         }
         //  Up to 16 bits of gray levels
         else if (maxVal  < 65536)
         {
             //  Allocate the pixel array
             unsigned short* pixelData = (unsigned short*)malloc(nbCols*nbRows*2);
             image.raster = pixelData;
             image.type = DEEP_GRAY_RASTER;
             image.maxVal = maxVal;
             image.bytesPerPixel = 2;
             image.bytesPerRow = 2*nbCols;
             //  Allocate raster2D
             unsigned short** r2D = (unsigned short**) calloc(nbRows, sizeof(unsigned short*));
             for (unsigned int i=0; i<nbRows; i++)
                 r2D[i] = pixelData + i*nbCols;
             image.raster2D = (void*) r2D;
             //  The image is stored in an ASCII format
             if (isASCII)
             {
                 for (unsigned int i = nbRows-1, invI = 0; invI < nbRows; i--, invI++)
                 {
                     unsigned index = i*nbCols;
                     
                     for (unsigned int j = 0; j < nbCols; j++, index++)
                     {
 //                      fscanf(inPPM, "%hu", pixelData + index);
                         fscanf(inPPM, "%hu", r2D[i] + j);
                     }
                     printf("\n");
                 }
             }
             //  The image is stored in a binary format
             else
             {
                 for (unsigned int i = nbRows-1, invI = 0; invI < nbRows; i--, invI++)
                 {
                     unsigned index = i*nbCols;
                     
                     for (unsigned j = 0; j < nbCols; j++, index++)
                     {
                         //  This is really ugly but "%hu" doesn't seem to work with
                         //  binary files.
                         char* vp = (char*) (r2D[i] + j);
                         fscanf(inPPM, "%c%c", vp, vp+1);
                     }
                 }
             }
         }
         else
         {
             printf("Only gray levels up to 16 bits currently supported.\n");
             exit(WRONG_FILE_TYPE);
         }
     }
     
     fclose(inPPM);
     
     //  Copy the information into the image struct
     image.width = nbCols;
     image.height = nbRows;
     
     return image;
 }
 //----------------------------------------------------------------------
 //  writePPM_PGM()
 //  Goal :  writes an image in PPM or PGM binary format
 //  Input:  path to the output file
 //          pointer to the image to write
 //
 //  Returns: Error code (0 = no error)
 //
 //  Note: Apparently PPM image files are always written upside-down
 //----------------------------------------------------------------------
 ImageIOErrorCode writePPM_PGM(const char* filePath, ImageStruct* img)
 {
     if (filePath == NULL)
     {
         printf("Error in writePPM: file name == NULL\n");
         return FILE_PATH_NULL;
     }
     FILE* fp = fopen(filePath, "wb");
     if (fp == NULL)
     {
         printf("Error in writePPM: couldn't write to file %s\n", filePath);
         return CANNOT_OPEN_WRITE_FILE;
     }
     if (img->type == RGBA32_RASTER)
     {
         fprintf(fp, "P6\n");
         fprintf(fp, "%d %d\n", img->width, img->height);
         fprintf(fp, "255\n");
         int** raster2D = (int**) img->raster2D;
         
         for (unsigned int i=img->height-1, mirrorI=0; mirrorI<img->height; i--, mirrorI++)
         {
             for (unsigned int j=0; j<img->width; j++)
             {
                 size_t ierr = fwrite(raster2D[i]+j, 1, 3, fp);
                 
                 if (ierr != 3)
                 {
                     fprintf(stderr, "Error in writePPM: couldn't write to file %s\n",
                                      filePath);
                     fclose(fp);
                     return CANNOT_WRITE_FILE;
                 }
             }
         }
     }
     else if (img->type == GRAY_RASTER)
     {
         fprintf(fp, "P5\n");
         fprintf(fp, "%d %d\n", img->width, img->height);
         fprintf(fp, "255\n");
         unsigned char** raster2D = (unsigned char**) img->raster2D;
         
         for (unsigned int i=img->height-1, mirrorI=0; mirrorI<img->height; i--, mirrorI++)
         {
             size_t ierr = fwrite(raster2D[i], 1, img->width, fp);
                 
             if (ierr != 1)
             {
                 fprintf(stderr, "Error in writePPM: couldn't write to file %s\n",
                                  filePath);
                 fclose(fp);
                 return CANNOT_WRITE_FILE;
             }
         }
     }
     else if (img->type == DEEP_GRAY_RASTER)
     {
         fprintf(fp, "P5\n");
         fprintf(fp, "%d %d\n", img->width, img->height);
         fprintf(fp, "%d\n", img->maxVal);
         unsigned short** raster2D = (unsigned short**) img->raster2D;
         
         for (unsigned int i=img->height-1, mirrorI=0; mirrorI<img->height; i--, mirrorI++)
         {
             size_t ierr = fwrite(raster2D[i], 2, img->width, fp);
             
             if (ierr != img->width)
             {
                 fprintf(stderr, "Error in writePPM: couldn't write to file %s\n",
                                  filePath);
                 fclose(fp);
                 return CANNOT_WRITE_FILE;
             }
         }
     }
     else
     {
         printf("A float raster image cannon be saved in PPM/PGM format\n");
         return WRONG_FILE_TYPE;
     }
     
     fclose(fp);
     return NO_ERROR;
 }
 char getHeaderInfoPPM_PGM_(FILE* inPPM, const char* filePath,
                            ImageFileType* fileType,
                            unsigned int* nbRows, unsigned int* nbCols,
                            unsigned int* maxVal)
 {
     char    buf[80], skip[80], imgType[2], c;
     int     commentsSkipped = 0;
     
     // --- find if we have a known picture format ---
     fscanf(inPPM, "%s", buf);
     imgType[0] = buf[0];
     imgType[1] = buf[1];
     
     if ((imgType[0] != 'P') || ((imgType[1] != '2') && (imgType[1] != '5') &&
                                 (imgType[1] != '3') && (imgType[1] != '6')))
     {
         printf("%s is not a file in the PPM format.\n", filePath);
         exit(WRONG_FILE_TYPE);
     }
     //  Get the images type
     switch (imgType[1])
     {
         case '2':
         case '5':
             *fileType = kPGM;
             break;
         case '3':
         case '6':
             *fileType = kPPM;
             break;
         default:
             break;
     }
     
     //  Note: I am doing some really ugly stuff with file input and buffers.
     //  I am not sure if I had to do that because ANSI file I/O libraries
     //  on the Mac are screwed up (they might be, for all I know), or just
     //  because I kept missing something important somewhere.
     //  skip whitespace
     fscanf(inPPM, "%[\n\r\t ]", skip);
     //  read until we encounter a non-whitespace character.  This could be a comment
     //  or the nb of columns
     fscanf(inPPM, "%[^\n\r\t ]", buf);
     
     if (buf[0] != '#')
         commentsSkipped = 1;
     
     while (!commentsSkipped)
     {
         //  if we stepped into  some comment...
         if (buf[0] == '#')
         {
             //  ...skip the rest of the line
             fscanf(inPPM, "%[^\n\r]", skip);
             //  skip additional whitespace on the next line
             fscanf(inPPM, "%[\n\r\t ]", skip);
             
             //  then read until the next whitespace
             fscanf(inPPM, "%[^\n\r\t ]", buf);
         }
         else
             commentsSkipped = 1;
     }
     //  but should contain only one thing: the width of the image
     sscanf(buf,"%d", nbCols);
     
     //  the rest of the info is read from the file stream
     fscanf(inPPM,"%d", nbRows);
     fscanf(inPPM,"%d", maxVal);
     
     //  Read the extra space
     fscanf(inPPM, "%c", &c);
     
     if (imgType[1] == '2' || imgType[1] == '3')
         return 1;
     else
         return 0;
 }


//
//  dimensions.c
//  Prog 02 Solution
//
//  Created by Jean-Yves Hervé on 2018-10-03.
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
typedef enum OptionalVal
	{
		NO_OPTIONAL = -1,
		//
		WIDTH_OUT = 0,
		HEIGHT_OUT,
		VERBOSE_OUT,
		//
		NUM_OPTIONALS	//	automatically 3
		
	} OptionalVal;

char* OptionalStr[NUM_OPTIONALS] = {"-w", "-h", "-v"};


#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Function prototypes
//-------------------------------------------------------------------
#endif

/**	Function that converts an argument into an optional value
 *	@param	arg		an argument to the program (a C string)
 *	@return		a code for the optional if it was regonized
 */
OptionalVal parseArgument(char* arg);

/**	Function that goes over all but last arguments to see if
 *	any is an optional
 *	@param argc		number of arguments passed to the program, plus 1
 *	@param argv		lists of arguments passed to the program (C strings)
 *	@param option	array of "booleans" for the various options.
 *	@return		an error code (0 for no error)
 */
int parseArgumentList(int argc, char* argv[], int option[]);

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Function implementations
//-------------------------------------------------------------------
#endif

//--------------------------------------------------------------
//	Main function, expecting as arguments:  [-h|w] [-v] path
//	It returns an error code (0 for no error)
//--------------------------------------------------------------
int main(int argc, char* argv[])
{
	//	we need at least one argument
	if (argc < 2)
	{
		printf("Proper usage: dimensions [-h|w] [-v] path\n");
		return 1;
	}

	//	some "booleans" for output format
	int option[NUM_OPTIONALS] = {0, 0, 0};
	
	//	First, process the optionals
	int err = parseArgumentList(argc, argv, option);
	if (err)
		return err;
	
	//	So now at this point we have set up our output flags and we can look
	//	at the image.
	int width, height;
	
	//	I provide two versions of the code.
	#if 1
		//	In the first version, I simply read the image into a RasterImage struct and
		//	report the dimensions from the struct.  One could criticize this version
		//	on account that it wastes time reading the image and also allocates space
		//	for no good reason, even if we free it right after that.
		{
			//	I do this within bracket so that my ImageStruct gets freed.
			//	Of course, I have to free the dynamically-allocated raster myself
			RasterImage image = readTGA(argv[argc-1]);
			width = image.numCols;
			height = image.numRows;
			//	Cleanup allocations.  Again, this is not really needed, since the full
			//	partition will get cleared when the process terminates, but I like to
			//	keep the good habit of freeing memory that I don't need anymore, and,
			//	if I crash, it's a sign that something went wrong earlier and I may
			//	have produced junk
			deleteRasterImage(&image);
		}
	#else
		//	In the second version, I copy the header-reading code from readTGA to
		//	only extract the dimensions.  This option may pose problem if the code
		//	has constraining licensing conditions.  It it also more prone to error
		//	since I have to edit the code.  On the other hand it is more efficient.
		//	Also, this allows me to customize possible errors messages.  In the
		//	first version, I am completely dependent on the error messages of readTGA.
		//
		//	***Code based on partial copy of readTGA function, in imageIO_TGA.c***
		{
			FILE* tga_in = fopen(argv[argc-1], "rb" );
			if (tga_in == NULL)
			{
				printf("Cannot open image file %s\n", argv[argc-1]);
				exit(11);
			}

			//--------------------------------
			//	Read the header (TARGA file)
			//--------------------------------
			char	head[18] ;
			fread( head, sizeof(char), 18, tga_in ) ;
			/* Get the size of the image */
			width = (int)(((unsigned int)head[12]&0xFF) | (unsigned int)head[13]*256);
			height = (int)(((unsigned int)head[14]&0xFF) | (unsigned int)head[15]*256);
		}
	
	#endif
	
	//	Output the dimensions
	if (option[WIDTH_OUT])
	{
		if (option[VERBOSE_OUT])
			printf("width: ");
		printf("%d", width);
		if (option[HEIGHT_OUT])
		{
			if (option[VERBOSE_OUT])
				printf(",");
			printf(" ");
		}
	}
	if (option[HEIGHT_OUT])
	{
		if (option[VERBOSE_OUT])
			printf("height: ");
		printf("%d", height);
	}
	printf("\n");
	
	return 0;
}

OptionalVal parseArgument(char* arg)
{
	OptionalVal val = NO_OPTIONAL;

	//	Check all possible values
	for (unsigned int k=0; (k<NUM_OPTIONALS) && (val == NO_OPTIONAL); k++)
	{
		if (strcmp(arg, OptionalStr[k]) == 0)
		{
			val = k;
		}
	}
	
	return val;
}

int parseArgumentList(int argc, char* argv[], int option[])
{
	for (unsigned int k=1; k<argc-1; k++)
	{
		OptionalVal val = parseArgument(argv[k]);
		switch (val)
		{
			case WIDTH_OUT:
				if (!option[WIDTH_OUT])
					if (!option[HEIGHT_OUT])
						option[WIDTH_OUT] = 1;
					else
					{
						printf("Optional arguments -h and -w are mutually exclusive.\n");
						return 2;
					}
				else
				{
					printf("Optional argument -w cannot be repeated.\n");
					return 3;
				}
			break;

			case HEIGHT_OUT:
				if (!option[HEIGHT_OUT])
					if (!option[WIDTH_OUT])
						option[HEIGHT_OUT] = 1;
					else
					{
						printf("Optional arguments -h and -w are mutually exclusive.\n");
						return 4;
					}
				else
				{
					printf("Optional argument -h cannot be repeated.\n");
					return 5;
				}
			break;

			case VERBOSE_OUT:
				if (!option[VERBOSE_OUT])
					option[VERBOSE_OUT] = 1;
				else
				{
					printf("Optional argument -v cannot be repeated.\n");
					return 6;
				}
			break;

			default:
				//	Invalid argument
				printf("Invalid argument: %s is not a valid option\n", argv[k]);
				return 7;
				break;

		}
	}
	
	//	If the user didn't specify width or height, we set both for output
	if (!option[WIDTH_OUT] && !option[HEIGHT_OUT])
		option[WIDTH_OUT] = option[HEIGHT_OUT] = 1;

	return 0;
}


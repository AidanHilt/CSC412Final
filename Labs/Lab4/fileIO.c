/*--------------------------------------------------------------+
|		A simple C program to demonstrate some basic elements	|
|	of the language to Java programmers.						|
|																|
|	Jean-Yves Hervé, 3DG URI									|
|	2019-09-03													|
+--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------
//	Function prototypes
//-------------------------

void part1(void);
void part2(void);
void part3(void);
void part4(void);
void getNextLine(FILE* inFile, char* line, int sizeLine);


//-------------------------
//	Some constants
//-------------------------
#define INPUT_FILE_PATH_1	"./Data/input1.dat"
#define INPUT_FILE_PATH_2	"./Data/input2.dat"
#define INPUT_FILE_PATH_3	"./Data/input3.dat"
#define INPUT_FILE_PATH_4	"./Data/input4.dat"
#define INPUT_FILE_PATH_5	"./Data/input5.dat"
#define INPUT_FILE_PATH_6	"./Data/input6.dat"
#define OUTPUT_FILE_PATH_A	"./Output/outA.txt"
#define OUTPUT_FILE_PATH_B	"./Output/outB.txt"

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------


int main(int argc, char* argv[])
{
	part1();
	
	part2();
	
	part3();

	part4();

	return 0;
}

//-----------------------------------------------------------------------------------
//	Part 1: Basic file I/O
//-----------------------------------------------------------------------------------
void part1(void)
{
	printf("\n\n-------------------\nPart 1\n-------------------\n");
	
	//----------------------------------
	//	File input: ASCII (text) file
	//----------------------------------
	FILE *inFile = fopen(INPUT_FILE_PATH_1, "r");
	
	//	make sure that the file was properly open.  It may not exist (bad path), or
	//	it may be busy or not accessible (access rights)
	if (inFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\"\n", INPUT_FILE_PATH_1);
		exit(11);
	}
	
	//	I read 2 int, a float, and a string (no space) from the file
	//	Note that I need to know the maximum size of my string (+1 for the terminating 0)
	int a, b;
	float x;
	char s[81];
	
	//	We specify the source, the format of the data, and the location were we want
	//	to read the data into.
	//	The C string s is a pointer to the beginning of the string, so we should not
	//	pass &s
	int nRead = fscanf(inFile, "%d%d%f%s", &a, &b, &x, s);
	
	//	fscanf returns the number of elements that were successfully read.  So it's a 
	//	good idea to check that number
	if (nRead != 4)
	{
		printf("Something went wrong while ready the data. Only %d elements were read\n", nRead);
		exit(12);
	}
	printf("I read: a=%d, b=%d, x=%f, s=\"%s\"\n", a, b, x, s);
	
	//	don't forget to close the file when you're done
	fclose(inFile);
	
	//	Note that in the format string of the fscanf statement, "blank" characters (space
	//	and tab) are skipped.  So, I could have written 
	//			fscanf(inFile, "%d %d %f %s", &a, &b, &x, s);
	//	or even
	//			fscanf(inFile, "%d  %d\t%f \t %s", &a, &b, &x, s);
	//	It's  a different story when the format uses non-blank characters, as we shall
	//	see in Part 3.
	
	//----------------------------------
	//	File output: ASCII (text) file
	//----------------------------------
	FILE* outFile = fopen(OUTPUT_FILE_PATH_A, "w");

	//	Again, make sure that the file was properly open.  Here, the first time you
	//	run the program, it will fail because the directory Output doesn't exist.
	//	Create that directory (mkdir Output) and run again.
	if (outFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\" for writing\n", OUTPUT_FILE_PATH_A);
		exit(13);
	}

	//	I write  back to the file the data in the same form as in the input file.
	//	Note that here I need to include the separating spaces in the format string.
	//	Also note that I don't pass a pointer to a, b, x but the *values* of these variables.
	fprintf(outFile, "%d %d %f %s\n", a, b, x, s);
	
	//	Of course, since fprintf works exactly like printf in terms of formatting, I could
	//	also produce an output file more convenient for human consumption.
	fprintf(outFile, "a=%d, b=%d, x=%6.2f, s=%s\n", a, b, x, s);
	
	//	again, don't forget to close the file
	fclose(outFile);
	
	printf("Output of ASCII file completed.\n");
	
	//	If you checked what the contents of the input1.dat and outputA.txt files looked 
	//	like, you saw that it stored the numbers as text, so 246.75 occupies 6 characters
	//	(that is, 6 bytes).  One character per digit, plus one for the decimal dot, plus
	//	one space between separate numbers.
	//	The advantage of ASCII file output is that the files are human-readable.  The
	//	drawback is that they take a lot more space: a float is stored on 4 bytes and a
	//	char (sufficient to store 1 color channel of a pixel in an image) only needs 1 byte,
	//	as opposed to 4 (3 in average for the data, plus one for the separating space) in
	//	ASCII form.  For this reason, large data sets are often stored on files in
	//	"binary form," with no space separation between the data.  The continuous sequence
	//	of bytes is divided into words by the native byte size of what is being read.
	
	//	note the wb --> write binary
	outFile = fopen(OUTPUT_FILE_PATH_B, "wb");
	if (outFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\" for writing\n", OUTPUT_FILE_PATH_B);
		exit(14);
	}
	
	//	To write into a binary file, you must use fwrite rather than fprintf
	//	fwrite writes bytes.  The format is: 
	//
	//	fwrite(startAddr, dataSize, numData, filePtr);
	//				^		^			^		^
	//				|    	|			|		|
	//				|		|			|		+--- file pointer
	//				|		|			+-- how many data of this type/size
	//				|		+---- byte size of one element
	//				+---- start address of the element(s) to write to the file
	
	//	So, if I want to write the same kind of data (2 ints, 1 float, a string), but this
	//	time in binary form, I would also need to write the size of the string (an
	//	advantage of this is that my string can now include spaces and tabs)
	int sLength = strlen(s);
	fwrite(&a, sizeof(int), 1, outFile);
	fwrite(&b, sizeof(int), 1, outFile);
	fwrite(&x, sizeof(float), 1, outFile);
	fwrite(&sLength, sizeof(int), 1, outFile);	
	fwrite(s, sizeof(char), sLength, outFile);
	fclose(outFile);
	printf("Output of binary file completed.  Now reading binary...\n");

	//	read binary 
	inFile = fopen(INPUT_FILE_PATH_2, "rb");	
	//	make sure that the file was properly open.  It may not exist (bad path), or
	//	it may be busy or not accessible (access rights).
	//	The first time you run the program, it's going to fail because the data file 
	//	doesn't exist.  
	//	Execute
	//		cp Output/outB.txt Data/input2.dat
	//	and run the program again.
	if (inFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\"\n", INPUT_FILE_PATH_2);
		exit(15);
	}
	
	//	And now, to read, we use fread rather that fscanf.  The format is the same as that
	//	of fwrite.  fread returns the number of bytes successfully read.  Note that fread
	//	won't check if the bytes read "make sense" in terms of the type of the variable 
	//	into which you are reading the bytes.  So, in practice, the value returned by fread
	//	is not quite as useful as that returned by fscanf.  It just informs you that you
	//	may have encountered the end of the file earlier than expected.
	//	Here, I read my data into "fresh" new variables
	int a2, b2, sLength2;
	float x2;
	char* s2;

	fread(&a2, sizeof(int), 1, inFile);	
	fread(&b2, sizeof(int), 1, inFile);	
	fread(&x2, sizeof(float), 1, inFile);	
	fread(&sLength2, sizeof(int), 1, inFile);
	//	Now I can allocate my string at just the right size, not forgetting the space for
	//	the 0-terminating char.  I calloc to make sure that my final char is 0.
	s2 = (char*) calloc(sLength2+1, sizeof(char));
	fread(s2, sizeof(char), sLength2, outFile);		
	printf("I read: a2=%d, b2=%d, x2=%f, s2=\"%s\"\n", a2, b2, x2, s2);
	
	//	don't forget to close the file when you're done
	fclose(inFile);
}

//-----------------------------------------------------------------------------------
//	Part 2: Read data into an array
//-----------------------------------------------------------------------------------
void part2(void)
{
	printf("\n\n-------------------\nPart 2\n-------------------\n");
	
	//----------------------------------
	//	File input: ASCII (text) file
	//----------------------------------
	FILE *inFile = fopen(INPUT_FILE_PATH_3, "r");
	if (inFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\"\n", INPUT_FILE_PATH_3);
		exit(21);
	}

	//	The file stores on a first line the number of rows and columns of the array
	unsigned int numRows, numCols;
	int nRead = fscanf(inFile, "%u%u", &numRows, &numCols);
	if (nRead != 2)
	{
		printf("Something went wrong while ready the data.  Only reads %d element.\n", nRead);
		exit(22);
	}
	
	//	Allocate an array for the data
	float** a = (float**) calloc(numRows, sizeof(float*));
	for (unsigned int i=0; i<numRows; i++)
		a[i] = calloc(numCols, sizeof(float));
		
	//	Now we read, row by row
	for (unsigned int i=0; i<numRows; i++)
		for (unsigned int j=0; j<numCols; j++)
			fscanf(inFile, "%f", a[i]+j);
	
	//	close the file
	fclose(inFile);
			
	//	Print out what we read
	printf("Array read from ASCII file (%d rows and %d columns:)\n", numRows, numCols);
	for (unsigned int i=0; i<numRows; i++)
	{
		printf("\t");
		for (unsigned int j=0; j<numCols; j++)
			printf("%8.3f  ", a[i][j]);
		printf("\n");
	}	
	
	//	Bonus:  I write a to a binary file to produce input4.dat
	FILE* outFile = fopen(INPUT_FILE_PATH_4, "wb");
	if (outFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\" for writing\n", INPUT_FILE_PATH_3);
		exit(23);
	}
	fwrite(&numRows, sizeof(unsigned int), 1, outFile);
	fwrite(&numCols, sizeof(unsigned int), 1, outFile);
	//	Be careful that the different rows of a may not be stored next to each other.
	//	On the other hand, each row is of course stored contiguously
	for (unsigned int i=0; i<numRows; i++)
		fwrite(a[i], sizeof(float), numCols, outFile);
	fclose(outFile);	
	
	//----------------------------------
	//	File input: binary file
	//----------------------------------
	inFile = fopen(INPUT_FILE_PATH_4, "rb");
	if (inFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\"\n", INPUT_FILE_PATH_4);
		exit(24);
	}
	
 	//	This time again, I use "fresh" variable to make sure that I am indeed reading
	unsigned int numRows2, numCols2;
	
	fread(&numRows2, sizeof(unsigned int), 1, inFile);
	fread(&numCols2, sizeof(unsigned int), 1, inFile);

	//	Allocate an array for the data
	float** a2 = (float**) calloc(numRows2, sizeof(float*));
	for (unsigned int i=0; i<numRows2; i++)
		a2[i] = calloc(numCols2, sizeof(float));

	//	Same as  for the writing, we read a full row at a time
	for (unsigned int i=0; i<numRows2; i++)
		fread(a2[i], sizeof(float), numCols2, inFile);
		
	//	Print out what we read
	printf("Array read from binary file (%d rows and %d columns:)\n", numRows2, numCols2);
	for (unsigned int i=0; i<numRows2; i++)
	{
		printf("\t");
		for (unsigned int j=0; j<numCols2; j++)
			printf("%8.3f  ", a[i][j]);
		printf("\n");
	}	
			
	//	presumably I would be doing something with the data
	//	...
	//	...
	
	//	Unless I return the array, I must free it before leaving, to avoid leaking memory
	for (unsigned int i=0; i<numRows; i++)
	{
		free(a[i]);
		free(a2[i]);
	}
	free(a);	
	free(a2);	
}

//-----------------------------------------------------------------------------------
//	Part 3: Text files with comments
//-----------------------------------------------------------------------------------
void part3(void)
{
	printf("\n\n-------------------\nPart 3\n-------------------\n");
	
	//----------------------------------
	//	File input: ASCII (text) file
	//----------------------------------
	FILE *inFile = fopen(INPUT_FILE_PATH_5, "r");
	if (inFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\"\n", INPUT_FILE_PATH_5);
		exit(21);
	}

	//	Some text file format allow for comments in the data.  Typically, a comment line
	//	would start with a # character, like in bash.
	//	When you read such files, you want to skip the comments.  The simplest way to do 
	//	this is to read the line into a C string, check the first character of the 
	//	string.  If it is a # char, then read the next line.  If the first character
	//	is not a #, then sscanf the bits you need.
	
	//	In C, there is no choice but to read into a big array of char.  So you need to have
	//	some idea of the maximum size of the line
	char line[200];
	
	getNextLine(inFile, line, sizeof(line));
	
	//	and now I process my line \, replacing fscanf by sscanf
	unsigned int numRows, numCols;
	int nRead = sscanf(line, "%u%u", &numRows, &numCols);
	if (nRead != 2)
	{
		printf("Something went wrong while ready the data.  Only reads %d element.\n", nRead);
		exit(22);
	}

	//	Allocate an array for the data
	float** a = (float**) calloc(numRows, sizeof(float*));
	for (unsigned int i=0; i<numRows; i++)
		a[i] = calloc(numCols, sizeof(float));
		
	//	Now we read, row by row
	for (unsigned int i=0; i<numRows; i++)
	{
		getNextLine(inFile, line, sizeof(line));

		for (unsigned int j=0; j<numCols; j++)
			sscanf(line, "%f", a[i]+j);
		
	}
	
	//	close the file
	fclose(inFile);
			
	//	Print out what we read
	printf("Array read from ASCII file (%d rows and %d columns:)\n", numRows, numCols);
	for (unsigned int i=0; i<numRows; i++)
	{
		printf("\t");
		for (unsigned int j=0; j<numCols; j++)
			printf("%8.3f  ", a[i][j]);
		printf("\n");
	}	

}


//-----------------------------------------------------------------------------------
//	Part 4: Format issues
//-----------------------------------------------------------------------------------
void part4(void)
{
	printf("\n\n-------------------\nPart 4\n-------------------\n");
	
	//	Some ASCII files may contain formatted data produced by another program,
	//	or simply data written to be easier to read by a user.
	FILE* inFile = fopen(INPUT_FILE_PATH_6, "r");
	if (inFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\"\n", INPUT_FILE_PATH_6);
		exit(31);
	}
	
	//	Line 1
	//----------
	//	Let's start simple.  The line is 123+44.  We want to extract the two numbers
	int x, y;
	//	please note the + between the two %d in the format string
	int nRead = fscanf(inFile, "%d+%d", &x, &y);
	if (nRead != 2)
		printf("Something went wrong, read %d number(s).\n", nRead);
	else
		printf("Read x=%d and y=%d\n", x, y);

	//	Line 2
	//----------
	//	However, for whatever reason, The format string must start with a %
	//	Keeping that in mind, we can parse some fairly complex expressions
	//	The line is 69+(42+13)*(23+19).  No spaces around.
	int a, b, c, d, z;
	nRead = fscanf(inFile, "%d+(%d+%d)*(%d+%d)", &z, &a, &b, &c, &d);
	if (nRead != 5)
		printf("Something went wrong, read %d number(s).\n", nRead);
	else
		printf("Read z=%d, a=%d, b=%d, c=%d, and d=%d\n", z, a, b, c, d);

	//	Line 3
	//----------
	//	
	//	blank characters in the format string *do* matter, sometimes.
	//	For example, if your input string is "82+ 17", then you can
	//	read it with the format "%d+%d". The first %d will extract 82, next
	//	the '+' is found, which leaves " 17" to be processed, and the scanf 
	//	with format %d will "eat" the space
	//	On the other hand, if your input string is "82 +17", the format string
	//	cannot be "%d+%d", because, after the 82 has been read, the next character in
	//	the string wouldn't be '+', as announced in the format, but ' '.  On the 
	//	other hand, the formats string "%d +%d" would work.
	//	If the inout string is "82 + 17", then either "%d + %d" or "%d +%d" would work.
	nRead = fscanf(inFile, "%d + %d", &x, &y);
	if (nRead != 2)
		printf("Something went wrong, read %d number(s).\n", nRead);
	else
		printf("Read x=%d and y=%d\n", x, y);

	//	Line 4
	//	As long as the user respects the format, we can use human-readable input files.
	//	The line is 45 rows, 33 columns.
	nRead = fscanf(inFile, "%d rows, %d columns", &a, &b);
	//	The line is rows=45, cols=33.  Please note the space
//	nRead = fscanf(inFile, "rows=%d, cols=%d", &a, &b);
	if (nRead != 2)
		printf("Something went wrong, read %d number(s).\n", nRead);
	else
		printf("Read a=%d, b=%d\n", a, b);
	
}

void getNextLine(FILE* inFile, char* line, int sizeLine)
{
	int isGoodLine = 0;
	
	while (!isGoodLine)
	{
		fgets(line, sizeLine, inFile);
		if (line[0] != '#')
			isGoodLine = 1;
			
		//	just for debugging purposes
		else
			printf("Comment line eaten: %s\n", line);
			
	}
}

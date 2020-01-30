//---------------------------------------------------
//	A dumb  little "add two numbers" C program to
//	be used in Lab 01
//---------------------------------------------------

#include <stdio.h>

//	Performs no checking of any sort.  The script is supposed to do that.
int main(int argc, char* argv[])
{
	int a, b;
	sscanf(argv[1], "%d", &a);
	sscanf(argv[2], "%d", &b);
	printf("C program: The sum of %d and %d is %d\n", a, b,  (a+b));
	
	return 0;
}
#include <iostream>
#include <typeinfo>

using namespace std;

#define REF_VERSION		1
#define DEBUG_LEVEL		3

//-----------------------------------------------------------------------
//	Custom data types
//-----------------------------------------------------------------------

//	C-style struct declaration
typedef struct Point {
	int x, y;
} Point;

//	C++ style struct declaration
using MyThing = struct {
	int x;
	float y;
} ;


typedef struct DualArray {
	int* a;
	float* b;
} DualArray;

//-----------------------------------------------------------------------
//	Function prototypes
//-----------------------------------------------------------------------
//	C++ supports function overloading
void myFunc(int* a, int* b);

#if REF_VERSION
	void myFunc(int&a, int b);
#else
	void myFunc(int a, int b);
#endif

Point& staleReference(int a, int b);
void someOtherFunc(Point& a, const Point& b);
void someOtherFunc(Point* a, const Point* b);
DualArray someFuncV1(int m, int n);
void** someFuncV2(int m, int n);
MyThing& myFunc(void);

// return value is the int array    float array is an "output" parameter
int*  someFuncV3(int m, int n, float** b);

// int and float arrays are output parameters
void  someFuncV4(int m, int n, int**a, float**b);


//-----------------------------------------------------------------------
//	Function implementations
//-----------------------------------------------------------------------


int main (int argc, char * const argv[]) {

	//========================================================================
	//	Part I:	Casting
	//========================================================================

	//	casting C-style
	double x = 12.657;
	int ix = (int) x;
	cout << "x = " << x << "\t\tix = " << ix << endl << endl;

	//	possible
	int* px = (int*) &x;
	//	the compiler complains, but will still do it
	int* px2 = (int*) ix;

	//	casting old-C++ style
	int ix2 = int(x);
	//	casting "modern" C++ style
	int ix3 = static_cast<int>(x);

	
	//========================================================================
	//	Part 2:	References
	//========================================================================
	int a = 123;
	int& ref1 = a;

//	int& myRef = 4;


    cout << "a = " << a << endl;
    cout << "ref1 = " << ref1 << endl << endl;

	ref1 = 54;
	
    cout << "a = " << a << endl;
    cout << "ref1 = " << ref1 << endl << endl;

	//	must be initialized
//	int& invalid;
	//	a reference is not a pointer
//	int& ref2 = &a;

	//	but this works
	int* pa = &a;
	int* &ref3 = pa;
	*ref3 = 20;
	
	//	but this will not work
	int* const &ref4 = &a;
	*ref4 = 55;

	//	const references don't care much about type, even if the size of the data are not
	//		the same.  Note that this is not a binary map but a conversion.
	const double &ref5 = a;
//	ref5 = 1.5;
	
    cout << "a = " << a << endl;
    cout << "ref5 = " << ref5 << endl << endl;

	//	what if the data is itself const
	const int ca = 1234;
	const int &refca1 = ca;
	const int* const &refpca1 = &ca;



	int var = 123;
	int var2 = 6;
	
	int &ref6 = var;
	ref6 = var2;


	myFunc(var, var2);
	
	cout << "address of var = " << (int*)var << endl;
	cout << "address of ref6 = " << (int*)ref6 << endl;
	
	int arr[3] = {4, 7, 10};
	int bbb[5];
	int* const &rarr = arr;
	int xxx = arr[5];
	int yyy = rarr[7];
	cout << "value of yyy = " << yyy << endl;
	
 	Point p = {100, 20};
 	Point q = {20, -5};
 	someOtherFunc(p, q);
 	someOtherFunc(&p, &q);
 	
 	Point& u = staleReference(3.,5);
 	delete &u;
	
    return 0;
}

MyThing& myFunc(void)
{
	MyThing* p = new MyThing;
	
	return *p;
}

Point& staleReference(int a, int b)
{
	Point pt = {a, b};
	pt.x = a;
	pt.y  =b;

	return pt;	
}


void someOtherFunc(Point& a, const Point& b)
{
	a.x += b.x;
	a.y += b.y;
}

void someOtherFunc(Point* a, const Point* b)
{
	a->x += b->x;
	a->y += b->y;
}


#if REF_VERSION
void myFunc(int&a, int b) {
	a += b;
}

#else

void myFunc(int a, int b)
{
	a += b;
}

#endif

int*  someFuncV3(int m, int n, float** b)
{
	int* a = new int[m];
	*b = new float[m+n];


	return a;
}

void**  someFuncV2(int m, int n)
{
	int* a = new int[m];
	float* b = new float[m+n];
	
	void** returnVal = new void*[2];
	returnVal[0] = a;
	returnVal[1] = b;

//	void* returnVal[2] = {a, b};

	return returnVal;
}


DualArray  someFuncV1(int m, int n)
{
	DualArray returnVal;
	
	returnVal.a = new int[m];
	returnVal.b = new float[m+n];

	return returnVal;
}








void myFunc(int* a, int* b) {
	*a += ++(*b);
	
	a+= 2;
//	a[4] = 5;
}




// a and b are i/o parameters.  c  is input parameters
void myFunc(int* a, int* b, int c) {
	*a += ++(*b) + c;
}


// write a function that receives 2 int (m and n) and produces 2 1D  arrays
//i array of int of size m and one of floatof size m+ n

// version 1: put the two output arrays into the return value



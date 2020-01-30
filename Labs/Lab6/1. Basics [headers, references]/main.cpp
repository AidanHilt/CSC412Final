#include <iostream>
#include <typeinfo>

using namespace std;

void myFunc(int&a, int& b);
void myFunc(int* a, int* b);

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
	//	there are limts, though.
	int* px2 = (int*) ix;

	//	casting old-C++ style
	int ix2 = int(x);
	//	casting "modern" C++ style
	int ix3 = static_cast<int>(x);
	
	
	
	//========================================================================
	//	Part 2:	References
	//========================================================================
	int a = 123;
	
	int &ref1 = a;
    cout << "a = " << a << endl;
    cout << "ref = " << ref1 << endl << endl;

	ref1 = 54;
	
    cout << "a = " << a << endl;
    cout << "ref = " << ref1 << endl << endl;

	//	must be initialized
//	int &invalid;
	//	a reference is not a pointer
//	int &ref2 = &a;
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
//	ref6 = var2;


	myFunc(&var, &var2);
	
	myFunc(var, var2);
//	myFunc((int*) &rect, NULL);
	
	cout << "address of var = " << (int*)var << endl;
	cout << "address of ref6 = " << (int*)ref6 << endl;
	
	int arr[3] = {4, 7, 10};
	int bbb[5];
	int* const &rarr = arr;
	int xxx = arr[5];
	int yyy = rarr[7];
	cout << "value of yyy = " << yyy << endl;
	
    return 0;
}


void myFunc(int&a, int& b) {
	a += ++b;
}

void myFunc(int* a, int* b) {
	*a += ++(*b);
}

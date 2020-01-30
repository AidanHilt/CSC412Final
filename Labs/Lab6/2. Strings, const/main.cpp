//
//  main.cpp
//  Strings
//

#include <iostream>
#include <string>
#include <cstring>

using namespace std;

int main(int argc, const char * argv[]) {

	//	C-style string
	char a[100] = "This is a C-style string";
	
	int la = strlen(a);
	cout << a  << " (length = " << la << ")" << endl;
	
	// using the copy operator
	string s = "This is a C++ string";
	
	// using explicitly the constructor
	string v("This is a C++ string");
	
	int ls = s.size();
	cout << s << " (length = " << ls << ")" << endl;


	//	Go from C to C++ style
	string t(a);
	
	// in C++ we can overload operators.  the string class overloads the = operator
	string w = a;
	
	// Go from C++ to C.style
	const char* ww = s.c_str();
	
	// for primitive data types (int char, etc.) const kinda the same as final in Java
	const int x = 20;
	//x += 3;
	// with automatic arrays --> same
	const int a2[] = {6, 7, 3};
	//a2[1]  = -100;
	
	//	With pointers, const is more subtle:
	//		A const on the left side of a star refers to the data pointed at
	//		A const on the right side of a star refers to the pointer itself
	const int* fp = a2;
	int* gp = const_cast<int*>(a2);
	gp[1] = -100;
	
	//	Now, this becomes fun with double (and beyond) pointers...
	int*hp = new int[8];
	
	const int *hp1 = hp;//new int[6];
	int const *hp2 = hp;//new int[6];
	int* const hp3 = hp;//new int[6];		// different from the previous 2
	
	hp1[3] = 100;
	hp2[3] = 100;
	hp3[3] = 100;
	
	hp1 = NULL;//new int[6];
	hp2 = new int[6];
	hp3 = new int[6];
	
	//kp points to a 4x6 array of int
	int** kp = new int*[4];
	for (int i=0; i<4; i++)
		kp[i] = new int[6];
	
	kp[2][3] = 45;
	kp[1] = new int[20];
	
	const int*const* kp1 = kp;
	//kp1[1][3] = 10;
	//kp1[3] = new int[7];
	kp1 = NULL;
	
	const int**const kp2 = (const int**const)(kp);
	kp2 = NULL;// no
	kp2[1] = NULL;
	kp2[2][3] = 100;
	
	int const*const* kp3 = kp;
	// same as kp1  just style preference
	
	int *const*const kp4 = kp;
	kp4[1][2] = 6;
	kp4[2] = new int[7];
	kp4 = NULL;
	
	// modify only data.  not kp5, not rows
	int*const*const kp5  = kp;
	kp5[1][1] = 33;
	kp5[1] = NULL;
	kp5 = NULL;
	
	// insert code here...
	std::cout << "The end!\n";
    return 0;
}

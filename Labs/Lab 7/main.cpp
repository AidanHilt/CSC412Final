//
//  main.cpp
//  Lab 07 - Proj
//
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

using MyStruct = struct
{
	int m;
	float x;
};

int main(int argc, const char * argv[])
{
	//	File Input reminder
	ifstream inFile;
	inFile.open("data.txt");
	cout << "Reading from the file... ";
	int n;
	inFile >> n;
	cout << n << " data points:" << endl;

	for (int k=0; k<n; k++)
	{
		float x;
		int m;
		inFile >> x >> m;
		cout << "\t" << x << "  " << m << endl;
	}
	
	//	String stream reminder (?)
	char s[] = "12 2.5 word";
	stringstream ss;
	ss << s;
	
	int m;
	float x;
	string w;
	ss >> m >> x >> w;
	cout << "Got from string stream: m=" << m << ", x=" << x <<
		 ", and w=" << w << endl;

	//	Other way to create a string stream
	string v = "24 -6.7 stuff";
	stringstream ss2(v);
	ss2 >> m >> x >> w;
	cout << "Got from the other string stream: m=" << m << ", x=" << x <<
		 ", and w=" << w << endl;

	return 0;
}

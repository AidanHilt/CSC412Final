//
//  main.cpp
//  Lab 07 Solution - Part 1
//	
//	In this program, I review some stuff that you are going to need for the
//	rest of the semester, concerning lists, vectors, pointers, and iterators.
//	As usual, the two lab sessions unfolded completely differently.  This code
//	is mostly a cleanup of what we discussed in the Thursday session.
//
//	Note:  In what follows, I use the capitalized word List as a generic term
//	to refer to what could be implemented as an STL vector/list/deque.
//
//	Jean-Yves Hervé, 2019-11-01

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
//
#include <unistd.h>

using namespace std;

#if 0
//------------------------------------------
#pragma mark -
#pragma mark Custom data types
//------------------------------------------
#endif

using MyStruct = struct
{
	int m;
	float x;
};

//	Reminder that I can define custom types to improve readability
//	of the code (well, maybe not in this case)
using StructList = list<MyStruct>;
using constStructListIter = list<MyStruct>::const_iterator;


#if 0
//------------------------------------------
#pragma mark -
#pragma mark Implementation
//------------------------------------------
#endif

int main(int argc, const char * argv[])
{
	//-------------------------------------------------------------
	//	File Input reminder
	//	I read data from my file into a list or vector of struct
	//-------------------------------------------------------------
	ifstream inFile;
	inFile.open("data.txt");
	cout << "Reading from the file... ";
	int n;
	inFile >> n;
	cout << n << " data points:" << endl;

	//	Here I try to show how to work with multiple Lists that refer to the
	//	same data.  If all were container<MyStruct>, they would be storing
	//	different **copies** of the data.
	//	dataL will be my "master" list.
	list<MyStruct> dataL;
	//	dataV is a way to access the elements of dataL, but through a vector
	//	(therefore using an index).
	vector<MyStruct*> dataV;
	//	evenDataL is a partial list that only refers to the elements with an even m field.
	//	This list is "read-only" (const modifier).  Just 'cause, to show how it's done.
	list<const MyStruct*> evenDataL;

	for (int k=0; k<n; k++)
	{
		//	read a data point into a struct object
		MyStruct s;
		inFile >> s.x >> s.m;
		
		//	note that, alternatively, I could have read into separate variables,
		//	and initialized the struct after that
		//int m;
		//float x;
		//inFile >> x >> m;
		//MyStruct s = {x, m};
		
		//	Either way, I push a *copy* of the struct on my list (at the tail)
		dataL.push_back(s);
	}

	//	I print what's on my  list.  Here, because I only read the values without
	//	modifying them, I should use a const_iterator.  I don't *have* to, but it is
	//	good coding practice.  This does not mean that the iterator is constant
	//	(after all, I do an iter++) but that what it refers to is constant.
	//	Note that there are "const" versions of begin() and end(), so use them when
	//	appropriate.
	cout << "List form" << endl;
	for (list<MyStruct>::const_iterator iter=dataL.cbegin(); iter!=dataL.cend(); iter++)
	{
		//	iterators use the pointer notation to dereference:
		//		-  iter is a const iterator refering to a struct object in the list
		//		-  *iter is that struct object in the list (but const: not modifiable)
		//		-  (*iter).x is the x field of that struct object
		//		   or... iter->m is the m field of that struct object
		cout << "\t" << (*iter).x << "  " << iter->m << endl;
	}

	//	Now I build my Lists of pointers.  I am going to iterate through my dataL list
	//	and push the pointers to elements of dataL on dataV.
	//	The type of dataV is vector<MyStruct*>, not vector<const MyStruct*>, because
	//	I want to be able to modify the structs that the pointers in dataV point to.
	//	Therefore, I cannot use const_iterator to iterate dataL: if I used const_iterator,
	//	*iter would be a const MyStruct and &(*iter) would be a const MyStruct*.
	for (list<MyStruct>::iterator iter=dataL.begin(); iter!=dataL.end(); iter++)
	{
		//	It's a bit confusing here, because this is mixing iterator and pointer
		//	syntax, which partially coincide.
		//		-  iter is an iterator refering to a struct object in the list
		//		-  *iter is that struct object in the list
		//		-  &(*iter) is the address of that struct object
		dataV.push_back(&(*iter));
		
		//	I only want (pointers to) elements with an even m field on evenDataL
		if (iter->m % 2 == 0)
			evenDataL.push_back(&(*iter));

	}

	//-------------------------------------------------------------
	//	list/vector access reminder:  I print their content
	//-------------------------------------------------------------
	//	Vector access by index
	cout << "Vector form" << endl;
	for (unsigned int k=0; k<dataV.size(); k++)
	{
		cout << "\t" << dataV[k]->x << "  " << dataV[k]->m << endl;
	}
	//	Vector access by iterator, like a list.  This time I use const_iterator
	//	because I don't modify the elements in the list.
	cout << "Vector form v2" << endl;
	for (vector<MyStruct*>::const_iterator iter=dataV.cbegin(); iter!=dataV.cend(); iter++)
	{
		//	iter is an iterator to a struct object in the list
		//	*iter is that struct object in the list
		cout << "\t" << (*(*iter)).x << "  " << (*iter)->m << endl;
	}
	//	print out the partial list.
	cout << "Even List" << endl;
	for (list<const MyStruct*>::const_iterator iter=evenDataL.cbegin(); iter!=evenDataL.cend(); iter++)
	{
		//	iter is an iterator to a struct object in the list
		//	*iter is that struct object in the list
		cout << "\t" << (*iter)->x << "  " << (*iter)->m << endl;
	}

	//-------------------------------------------------------------
	//	Verification that I didn't simply copy the data
	//	The diferent lists/vectors refer to the same elements.
	//-------------------------------------------------------------
	//	I modify one element through dataV
	dataV[2]->x = 12.45f;
	
	//	I modify the head of dataL
	dataL.begin()->x = 1234.5;
	
	cout << "-------------------------" << endl;
	cout << "Lists after modifications" << endl;
	cout << "-------------------------" << endl;

	cout << "Vector form" << endl;
	for (unsigned int k=0; k<dataV.size(); k++)
	{
		cout << "\t" << dataV[k]->x << "  " << dataV[k]->m << endl;
	}
	cout << "Vector form v2" << endl;
	for (vector<MyStruct*>::const_iterator iter=dataV.cbegin(); iter!=dataV.cend(); iter++)
	{
		//	iter is an iterator to a struct object in the list
		//	*iter is that struct object in the list
		cout << "\t" << (*(*iter)).x << "  " << (*iter)->m << endl;
	}
	cout << "List form" << endl;
	for (constStructListIter iter=dataL.cbegin(); iter!=dataL.cend(); iter++)
	{
		//	iter is an iterator to a struct object in the list
		//	*iter is that struct object in the list
		cout << "\t" << (*iter).x << "  " << iter->m << endl;
	}
	cout << "Even List" << endl;
	for (list<const MyStruct*>::const_iterator iter=evenDataL.cbegin(); iter!=evenDataL.cend(); iter++)
	{
		//	iter is an iterator to a struct object in the list
		//	*iter is that struct object in the list
		cout << "\t" << (*iter)->x << "  " << (*iter)->m << endl;
	}

	//	Summary:  What have we learned so far
	//	First, it is possible to have multiple lists/vectors that refer to the same
	//	data without duplicating the data.  One list/vector List can contain the actual
	//	data while the other(s) contain pointers to the data.
	//	Second, My dual list/vector is silly, only one "full" List should ever be used.
	//	Partial Lists (e.g. "selected objects") are fine, but be careful about object
	//	removal (what's coming next)
	

	//	CAVEAT:  There is one big danger with multiple lists that refer to the
	//	same data:  What happens if data is removed from one of the lists?
	//
	//	1. If an object is removed from dataL?
	//		dataL.remove(*(dataL.begin()));
	//	then dataV[0] becomes a dangling pointer (points to an object that doesn't
	//	exist anymore).  We need to remove the pointer from dataV (and evenDataL).
	//	However, removing elements from a vector is cumbersome and inefficient, so
	//	don't do it.
	//	So...
	//	What type of List should you use?
	//	When dealing with data set of moderate size (up to tens of thousands of elements)
	//		o if elements are never removed or added --> arrays
	//		o if elements can only be added or removed at the tail --> vector
	//		o if elements can only be added or removed at the head or tail --> deque
	//		o otherwise, list
	//
	//	...which brings up the issue of erase vs. remove
	//	remove works by value:  take out all elements of the List whose value match
	//							the one passed as argument
	//	erase works with an iterator or a range of iterators
	//	Note that std::vector only supports erase (but does it inefficiently anywhere
	//	other than the tail of the vector.  Similarly, std::deque only supports erase
	//	(but does it inefficiently anywhere other than the head or tail of the deque.

	//	In what follows, I assume that my vector dataV never existed and that I only
	//	have my "full" list dataL and the partial list of selected objects evenDataL;
	
	//	Let's say that I want to remove the head element from the master List
	//	First, get the pointer to this element (we will need it for evenDataL)
	list<MyStruct>::iterator targetIt = dataL.begin();
	MyStruct* targetPtr = &(*targetIt);
	
	//	Remove the pointer from the lists of pointers evenDataL.  If the pointer
	//	doesn't belong to that list, this does nothing.
	evenDataL.remove(targetPtr);
	// and now remove the object from the master list
	dataL.erase(targetIt);
	
	cout << "-------------------------" << endl;
	cout << "Lists after removal 1" << endl;
	cout << "-------------------------" << endl;

	cout << "List form" << endl;
	for (constStructListIter iter=dataL.cbegin(); iter!=dataL.cend(); iter++)
	{
		//	iter is an iterator to a struct object in the list
		//	*iter is that struct object in the list
		cout << "\t" << (*iter).x << "  " << iter->m << endl;
	}
	cout << "Even List" << endl;
	for (list<const MyStruct*>::const_iterator iter=evenDataL.cbegin(); iter!=evenDataL.cend(); iter++)
	{
		//	iter is an iterator to a struct object in the list
		//	*iter is that struct object in the list
		cout << "\t" << (*iter)->x << "  " << (*iter)->m << endl;
	}

	return 0;
}


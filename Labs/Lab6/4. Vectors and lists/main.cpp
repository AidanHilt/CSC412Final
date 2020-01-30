//
//  main.cpp
//  STL-1
//
//  Created by Jean-Yves Hervé on 2016-04-04.
//  edited 2019-10-21
//

#include <iostream>
//
#include <list>
#include <deque>
#include <vector>
#include <string>
//
#include "SillyClass.h"

using namespace std;

deque<SillyClass*> createDeque(void);

int main(int argc, const char * argv[]) {

	//--------------------------------------------------------------
	//	Part 1: Using an STL container with primitive data types [list]
	//--------------------------------------------------------------
	cout << endl << "Part 1\n-----------" << endl;
	list<int> lI;
	lI.push_back(5);
	lI.push_front(15);
	lI.push_back(42);
	lI.push_back(-6);
	lI.push_back(30);

	for (list<int>::const_iterator it=lI.begin(); it!=lI.end(); it++)
		//	*it dereferences the iterator
		cout << (*it) << "   ";
	cout << endl;

	//	multiply every element in the list by ten
	for (list<int>::iterator it=lI.begin(); it!=lI.end(); it++)
		//	*it dereferences the iterator
		(*it) *= 10;

	cout << "after multiply" << endl;
	for (list<int>::const_iterator it=lI.begin(); it!=lI.end(); it++)
		//	*it dereferences the iterator
		cout << (*it) << "   ";
	cout << endl;
	
	//	find largest element in the list
	list<int>::const_iterator largestIter = lI.begin();
	int largest = *largestIter;
	for (list<int>::const_iterator it=lI.begin(); it!=lI.end(); it++) {
		if (*it > largest) {
			largest = *it;
			largestIter = it;
		}
	}
	
	//	insert the value 100 after the largest element in the list
	lI.insert(++largestIter, 100);
	
	for (list<int>::iterator it=lI.begin(); it!=lI.end(); it++)
		cout << (*it) << "   ";
	
	cout << endl;

	//--------------------------------------------------------------
	//	Part 2: Using an STL container with primitive data types [vector]
	//--------------------------------------------------------------
	cout << endl << "Part 2\n-----------" << endl;
	vector<float> vF;
	vF.push_back(5.2f);
	vF.push_back(1.5f);
	vF.push_back(4.2f);
	vF.push_back(-64.5f);
	vF.push_back(30.1f);

	for (vector<float>::const_iterator it=vF.begin(); it!=vF.end(); it++)
		//	*it dereferences the iterator
		cout << (*it) << "   ";
	cout << endl;

	//	But vectors call also be accessed by index
	for(int i=0; i<vF.size(); i++)
		cout << vF[i] << "   ";
	cout << endl;
	
	//--------------------------------------------------------------
	//	Part 3: Using an STL container with objects
	//--------------------------------------------------------------
	cout << endl << "Part 3\n-----------" << endl;
	vector<SillyClass> vO;
	SillyClass obj1(4, 5), obj2(6, 10), obj3(7, 2);
	
	vO.push_back(obj1);
	vO.push_back(obj2);
	vO.push_back(obj3);
	
	obj1.setM(100);
	vector<SillyClass>::const_iterator it2 = vO.begin();
	cout << "obj1.m_ = " << obj1.getM() << "    in list m_ = " << (*it2).getM() << endl;
	
	
	cout << endl;
	
	//--------------------------------------------------------------
	//	Part 4: Using an STL container with references to objects  --> can't be done
	//--------------------------------------------------------------
//	cout << endl << "Part 4\n-----------" << endl;
//	list<SillyClass&> lR;
//	SillyClass obj4(4, 5), obj5(6, 10), obj6(7, 2);
	
//
//	dR.push_back(obj4);
//	dR.push_back(obj5);
//	dR.push_front(obj6);
//	
//	obj6.setM(100);
//	deque<SillyClass>::const_iterator it3 = dR.begin();
//	cout << "obj6.m_ = " << obj6.getM() << "    in deque m_ = " << (*it3).getM() << endl;
	
	//--------------------------------------------------------------
	//	Part 5: Using an STL container with pointers to objects
	//--------------------------------------------------------------
	cout << endl << "Part 5\n-----------" << endl;
	
	// the objects are only accesssible/deletable through the deque
	//	possible memory leak issues
	deque<SillyClass*> dP = createDeque();

	deque<SillyClass*>::const_iterator it3 = dP.begin();
	
    return 0;
}

deque<SillyClass*> createDeque(void)
{
	deque<SillyClass*> dP;
	
	SillyClass* obj4 = new SillyClass(4, 5);
	SillyClass* obj5 = new SillyClass(6, 10);
	SillyClass* obj6 = new SillyClass(7, 2);
	
	dP.push_back(obj4);
	dP.push_back(obj5);
	dP.push_front(obj6);
	
	obj6->setM(100);
	
	return dP;
}

//
//  main.cpp
//  Lab 09
//
//  Created by Jean-Yves Hervé on 2019-11-12.
//

#include <vector>
#include <iostream>
#include <cstdio>			// for printf
#include <cstdlib>			// for exit()
#include <pthread.h>		// for pthread_* calls
#include <unistd.h>			// for usleep()
#include <sys/types.h>		// for gettid() on Linux

using namespace std;

//  main_single.cpp
//  Threads with no mutex
//
//	This small program shows how a randomized sorting can proceed:
//		- generate a random index i in the array
//		- if elements at array[i] and array[i+1] are not properly ordered, swap them
//	until the array is sorted.
//
//	That last part, figuring out that the array is effectively ordered, is a bit iffy.
//	What I do right now is that if we haven't found out-of-order elements for a while
//	we consider that the array has been sorted.
//
//  Created by Jean-Yves Hervé on 2016-04-25.
//							   modified 2019-11-04


//----------------------------
//	Function prototypes
//----------------------------
void sortArray(void);
void randomizeArray(void);


//	this is the array that we are going to sort using a multithreaded
//	randomized, bubble sort-inspired algorithm
const int NUM_VALUES = 30;
vector<int> v;

//	Terminate if out-of-order elements haven't been found after that many
//	trials.  This is a completely made up
//	number.  Something better thought out, based on the size of the
//	array and number of threads would be better, but this is not meant
//	to be a real solution to the sorting problem.
#define ENOUGH_TRIALS 1500


int main()
{
	//	seed the random generator
	srand ((unsigned int) time(NULL));
	
	//	Generate a permutation of the integers 1..ARRAY_SIZE
	randomizeArray();
	pthread_t *sortThread;

	pthread_create(sortThread, NULL, sortArray, NULL);
	
	return 0;
}


//	This is my thread function.  I don't really have anything to
//	get back from it (my array is a global variable), so the
//	return value will be NULL.
void *sortArray(void *nothing)
{
	(void) nothing;
	int countSinceLastSwap = 0;
	do
	{
		//	pick a random position
		int i = rand() % (v.size()-1);
		
		//	if two elements are out of order
		if (v[i] > v[i+1])
		{
			//	swap the elements
			int temp = v[i];
			v[i] = v[i+1];
			v[i+1] = temp;
			// reset the counter;
			countSinceLastSwap = 0;
		}
		//	The elements were in order --> increment the counter
		else
		{
			countSinceLastSwap++;
		}

		//	if there was a swap, then printout the array
		if (countSinceLastSwap == 0)
		{
			for (int k=0; k< v.size(); k++)
				printf("%3d", v[k]);
			printf("]\n");
		}
	}
	while (countSinceLastSwap < ENOUGH_TRIALS);


}


//	This is not a particularly efficient way to generate a
//	random permutaiton of an array, but, again, this is not
//	the purpose of this code sample.
void randomizeArray(void)
{
	//	Initialize with values 1..ARRAY_SIZE
	for (int k=0; k<NUM_VALUES; k++)
		v.push_back(k+1);
	
	//	Repeat many times
	for (int i=0; i<3*NUM_VALUES; i++)
	{
		//	pick two random elements of the array and swap them
		int j = rand() % v.size();
		int k = rand() % v.size();
		if (i != j)
		{
			int temp = v[j];
			v[j] = v[k];
			v[k] = temp;
		}
	}

	printf("Initial array: [");
	for (int k=0; k< v.size(); k++)
		printf("%3d", v[k]);
	printf("]\n");
}


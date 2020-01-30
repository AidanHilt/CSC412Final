//
//  main.cpp
//  Statics and Threads
//
//  Created by Jean-Yves Hervé on 2016-04-25.
//	revised 2016-10-11
//	revised 2017-03-01
//	revised	2018-11-01

#include <stdio.h>			// for printf
#include <pthread.h>		// for pthread_* calls
#include <string.h>			// for strerror()
#include <unistd.h>			// for usleep()
#include <stdlib.h>			// for exit()
#include <sys/types.h>		// for gettid() on Linux

void* myFunc (void* argument);

#define THREAD_COUNT 6
#define LOOP_COUNT   10
#define SLEEP_TIME   100000

int computationFunc(int x)
{
	//	Be careful that this variable is static for the function, not for
	//	a particular thread.
	static  int count = 0;
	count++;
	
//	//	slow down count++
//	int newCount = count + 1;
//	usleep(SLEEP_TIME);
//	count = newCount;
	
	int val = x*x;

	printf("\t\t\t===> x = %d --> val = %d [count = %d]\n", x, val, count);
	return val;
}


// information to tell the thread what to do.  For all
//	purposes, this is the "main" function of the threads
//	that we are going to create.
void* myFunc(void* argument)
{
	//	I know that the argument I received is really a pointer to
	//	an int (because I wrote the code, or read the specs, and
	//	therefore I can cast the pointer received back to a pointer
	//	to int, and dereference that pointer.
	int tNum = *((int *) argument);

	printf("\tThread %d created\n\t================\n", tNum);

	//	My thread will do some silly calculations, store their results into
	//	an array, and return the head pointer of that array as return value
	//	of the thread function.  There are many other ways to return values.
	//	I could have allocated an array of <insert data type> and let the
	//	threads access that array through their own thread index (index for
	//	the process, not the tread id for the OS).
	int* v = (int*) calloc(4, sizeof(int));
	v[0] = tNum;
	v[1] = tNum*tNum;
	//	Here, because I used calloc, I don't need to initialize v[2] to 0
	v[3] = 1;

	for (int i = 0; i < LOOP_COUNT; i++)
	{
		printf ("\t\t--thread %d counting %d\n", tNum, i);
		//	 do something to elements of v
		v[2] += computationFunc(i);
		v[3] *= 2;
		//	the u in usleep would like to look like µ, for micro.
		//	This is a traditional cutie abbreviation from the
		//	Unix world.
		usleep (SLEEP_TIME);
	}

	printf ("\tThread %d done, leaving myFunc\n", tNum);

	//	Remember that the array has been allocated on the heap,
	//	so the recipient of this value will be responsible for
	//	the housecleaning when the array is not needed anymore.
	return (v);

} // end of threadFunction

int main (int argc, char* argv[])
{

	//	Again, the pthread_t data type is implementation dependent.  It could
	//	a simple unsigned int (or long) or a pointer to a struct.
	pthread_t threadID[THREAD_COUNT];
	
	// create the threads
	for (int i = 0; i < THREAD_COUNT; i++) {
		printf("About to create Thread %d\n", i);

		//	If you pass NULL as second parameter, then this set
		//	defaults parameters for the thread, and that includes
		//	a setting for the thread to be "joinable", i.e. not "detached"
		int result = pthread_create (threadID + i, NULL, myFunc, &i);
		
		usleep (SLEEP_TIME);

		if (result != 0)
		{
			printf ("could not pthread_create thread %d. %d/%s\n",
					 i, result, strerror(result));
			exit (EXIT_FAILURE);
		}
	}

	//	Wait for all the threads to terminate.  Note that we will join
	//	the threads in the order in which this loop runs, not in the
	//	order in which the threads effectively terminates.  This is
	//	because pthread_join is what is caled a "blocking" call.
	for (int i = THREAD_COUNT-1; i>=0; i--)
	{
		int* w;
		pthread_join(threadID[i], (void**) &w);
		
		printf("Thread %d joined. Its w array stored: %d, %d, %d, %d\n", i, w[0], w[1], w[2], w[3]);
		
		//	Don't forget that the array was allocated (on the heap) by the
		//	thread function and we need to free it so as not to leak memory
		//	Of course, I do it here just to enforce good habits because
		//	the array will get erased when the process terminates and its
		//	partition is cleared.  Still, please note that after we leave the
		//	loop, the variable w is not defined anymore, and we won't have
		//	any means anymore to access or free the array).
		free(w);
	}
	
	return EXIT_SUCCESS;

} // main



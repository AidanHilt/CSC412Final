//
//  main.cpp
//  Threads-2
//
//  Created by Jean-Yves Hervé on 2016-04-25.
//	revised 2016-10-11
//	revised 2017-03-01
//	revised 2017-10-15
//	revised	2018-11-01

#include <stdio.h>		   // for printf
#include <stdlib.h>        // for exit
#include <pthread.h>       // for pthread_* calls
#include <string.h>        // for strerror()
#include <unistd.h>        // for usleep()
#include <sys/time.h>

#define THREAD_COUNT 6

// A struct data type to pass information to the thread and
//	possibly get values back from it.
typedef struct ThreadInfo {
    pthread_t   threadID;
    int         index;
    int         numberToCountTo;
    int         detachYourself;
    int         sleepTime;      // in microseconds
} ThreadInfo;

#if 0
#pragma mark -
#pragma mark Function implementations
#endif

void* threadFunction (void* argument)
{
	//------------------------------------------------
	//	Part 1: Extract argument data
	//------------------------------------------------
	
	//	I cast back the argument to the proper pointer type
    ThreadInfo* info = (ThreadInfo *) argument;

	//	And print to the console
    printf ("\tthread %d, counting to %d, sleep time %dµs, detaching: %s\n",
            info->index, info->numberToCountTo, info->sleepTime,
            (info->detachYourself) ? "yes" : "no");

	//------------------------------------------------
	//	Part 2
	//------------------------------------------------
	//	For whatever reason I would like to record the time spent between
	//	thread creation and end.  So we start this calculation here.
	//	I use the gettimeofday function that should exist on any Unix-like
	//	platform.

	//	struct timeval {
	//		time_t      tv_sec;     /* seconds */
	//		suseconds_t tv_usec;    /* microseconds */
	//	};
	struct timeval startTime;

	//	struct timezone {
	//		int tz_minuteswest;     /* minutes west of Greenwich */
	//		int tz_dsttime;         /* type of DST correction */
	//	};
	struct timezone weAreHere;
	
	int timeErr = gettimeofday(&startTime, &weAreHere);
	if (timeErr != 0)
	{
		fprintf( stderr, "could not gettimeofday at start of thread %d. Error code %d --> %s\n",
				 info->index, timeErr, strerror(timeErr));
	}
	unsigned long startTimeMicroSec = startTime.tv_sec*1000000 + startTime.tv_usec;

	//------------------------------------------------
	//	Part 3
	//------------------------------------------------
	//	If the thread should be detached, do it.
    if (info->detachYourself)
	{
        int detachErr = pthread_detach (pthread_self());
        if (detachErr != 0)
		{
            fprintf( stderr, "could not detach thread %d. Error code %d --> %s\n",
                     info->index, detachErr, strerror(detachErr));
        }
    }

	
	//------------------------------------------------
	//	Part 4: Time to do some work
	//------------------------------------------------
    // now to do the actual "work" of the thread
    for (int i = 0; i < info->numberToCountTo; i++)
	{
        printf ("\t\tthread %d counting %d / %d\n",
				info->index, i, info->numberToCountTo);
        usleep (info->sleepTime);
    }

	//------------------------------------------------
	//	Part 5: Before leaving, check the time again
	//------------------------------------------------
	struct timeval endTime;

	//	presumably we haven't changed time zone while running this demo,
	//	so I recycle my timezone struct and error code variable.
	timeErr = gettimeofday(&endTime, &weAreHere);
	if (timeErr != 0)
	{
		fprintf( stderr, "could not gettimeofday at end of thread %d. Error code %d --> %s\n",
				 info->index, timeErr, strerror(timeErr));
	}

	unsigned long timeSpent = (endTime.tv_sec*1000000 + endTime.tv_usec) - startTimeMicroSec;
	unsigned long timeSpentSec = timeSpent / 1000000;
	unsigned long timeSpentRemainder = timeSpent % 1000000;
	unsigned long timeSpentMilliSec = timeSpentRemainder / 1000;
	unsigned long timeSpentMicroSec = timeSpentRemainder % 1000;

    printf( "\tthread %d done, and it took %ld s, %ld ms, and %ld µs getting there\n",
			info->index, timeSpentSec, timeSpentMilliSec, timeSpentMicroSec);

	//	I showed in the previous code sample how I could return an array.
	//	This time I return nothing.  Note that you can also use the
	//	argument struct to store any values you want to return.
    return (NULL);

} // threadFunction

int main (int argc, char *argv[])
{
    ThreadInfo threads[THREAD_COUNT];
    int errCode;

    // create the threads
    for (int i = 0; i < THREAD_COUNT; i++)
	{
	
		//	But first, initialize the ThreadInfo struct that will
		//	be sent as parameter to the thread function
        threads[i].index = i;
        threads[i].numberToCountTo = (i + 1) * 2;
        threads[i].detachYourself = (i % 2); // detach odd threads
        threads[i].sleepTime = 2000000 - 300000 * i;
        // (make high-indexed threads wait less between counts)
	
		printf("main function creating thread %d\n", i);

        errCode = pthread_create (&threads[i].threadID, NULL,
                                 threadFunction, threads + i);
        if (errCode != 0) {
            fprintf (stderr, 
                     "could not pthread_create thread %d. %d/%s\n",
                     i, errCode, strerror(errCode));
            exit (EXIT_FAILURE);
        }

    }

	//	At this point, all the threads have been created and
	//	are busy doing their thing
	//	Possibly, the main thread could have computations of its
	//	own to perform here.  This one doesn't.

    //	Now rendezvous with all the non-detached threads
    for (int i = 0; i<THREAD_COUNT; i++) {
        void* retVal;
        if (!threads[i].detachYourself) {
            errCode = pthread_join (threads[i].threadID, &retVal);
            if (errCode != 0) {
               fprintf (stderr, "error joining thread %d. %d/%s\n",
                         i, errCode, strerror(errCode));
            }
            else {
				printf ("joined with thread %d\n", i);
			}
        }
    }

    exit (EXIT_SUCCESS);

} // main



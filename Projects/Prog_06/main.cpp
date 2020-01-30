//
//  main.cpp
//  GL threads
//
//  Created by Jean-Yves Hervé on 2017-04-24, revised 2019-11-19
//

 /*-------------------------------------------------------------------------+
 |	A graphic front end for a grid+state simulation.						|
 |																			|
 |	This application simply creates a glut window with a pane to display	|
 |	a colored grid and the other to display some state information.			|
 |	Sets up callback functions to handle menu, mouse and keyboard events.	|
 |	Normally, you shouldn't have to touch anything in this code, unless		|
 |	you want to change some of the things displayed, add menus, etc.		|
 |	Only mess with this after everything else works and making a backup		|
 |	copy of your project.  OpenGL & glut are tricky and it's really easy	|
 |	to break everything with a single line of code.							|
 |																			|
 |	Current GUI:															|
 |		- 'ESC' --> exit the application									|
 |		- 'r' --> add red ink												|
 |		- 'g' --> add green ink												|
 |		- 'b' --> add blue ink												|
 +-------------------------------------------------------------------------*/

#include <cstdio>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
//
#include "gl_frontEnd.h"

//==================================================================================
//	Function prototypes
//==================================================================================
void displayGridPane(void);
void displayStatePane(void);
void initializeApplication(void);


//==================================================================================
//	Application-level global variables
//==================================================================================

//	Don't touch
extern int	GRID_PANE, STATE_PANE;
extern int	gMainWindow, gSubwindow[2];

//	The state grid and its dimensions
int** grid;
const int NUM_ROWS = 20, NUM_COLS = 20;

//	the number of live threads (that haven't terminated yet)
int MAX_NUM_TRAVELER_THREADS = 10;
int numLiveThreads = 0;

//	the ink levels
int MAX_LEVEL = 50;
int MAX_ADD_INK = 10;
int redLevel = 20, greenLevel = 10, blueLevel = 40;

//	ink producer sleep time (in microseconds)
const int MIN_SLEEP_TIME = 1000;
int producerSleepTime = 1000000;

//	Enable this declaration if you want to do the traveler information
//	maintaining extra credit section
TravelerInfo *travelList;

//Controls access to the grid
pthread_mutex_t travelerGridLock;

//Controls access to the list of
//TravelerInfo called travelList
pthread_mutex_t travelerListLock;

//Controls access to the red level
pthread_mutex_t redLock;

//Controls access to the green level
pthread_mutex_t greenLock;

//Controls access to the blue level
pthread_mutex_t blueLock;

//Controls access to the number storing the
//number of threads
pthread_mutex_t threadNumLock;

//The list of all the travelers
pthread_t *travelersList = new pthread_t[MAX_NUM_TRAVELER_THREADS];

//The list of all the producers
pthread_t *producersList = new pthread_t[9];

//==================================================================================
//	These are the functions that tie the simulation with the rendering.
//	Some parts are "don't touch."  Other parts need your intervention
//	to make sure that access to critical section is properly synchronized
//==================================================================================


void displayGridPane(void){
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[GRID_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render the grid.
	//
	//	You *must* synchronize this call.
	//
	//---------------------------------------------------------
	//drawGrid(grid, NUM_ROWS, NUM_COLS);
	//
	//	Use this drawing call instead if you do the extra credits for
	//	maintaining traveler information
	pthread_mutex_lock(&travelerGridLock);
	pthread_mutex_lock(&threadNumLock);

	drawGridAndTravelers(grid, NUM_ROWS, NUM_COLS, travelList);

	pthread_mutex_unlock(&travelerGridLock);
	pthread_mutex_unlock(&threadNumLock);
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

void displayStatePane(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[STATE_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render information
	//	about the state of the simulation.
	//
	//	You *must* synchronize this call.
	//
	//---------------------------------------------------------
	drawState(numLiveThreads, redLevel, greenLevel, blueLevel);
	
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

//------------------------------------------------------------------------
//	These are the functions that would be called by a traveler thread in
//	order to acquire red/green/blue ink to trace its trail.
//	You *must* synchronize access to the ink levels
//------------------------------------------------------------------------
//
int acquireRedInk(int theRed){
	int ok = 0;
	pthread_mutex_lock(&redLock);
	if (redLevel >= theRed){
		redLevel -= theRed;
		ok = 1;
	}
	pthread_mutex_unlock(&redLock);
	return ok;
}

int acquireGreenInk(int theGreen){
	int ok = 0;
	pthread_mutex_lock(&greenLock);
	if (greenLevel >= theGreen){
		greenLevel -= theGreen;
		ok = 1;
	}
	pthread_mutex_unlock(&greenLock);
	return ok;
}

int acquireBlueInk(int theBlue){
	int ok = 0;
	pthread_mutex_lock(&blueLock);
	if (blueLevel >= theBlue){
		blueLevel -= theBlue;
		ok = 1;
	}
	pthread_mutex_unlock(&blueLock);
	return ok;
}

//------------------------------------------------------------------------
//	These are the functions that would be called by a producer thread in
//	order to refill the red/green/blue ink tanks.
//	You *must* synchronize access to the ink levels
//------------------------------------------------------------------------
//
int refillRedInk(int theRed){
	int ok = 0;
	pthread_mutex_lock(&redLock);
	if (redLevel + theRed <= MAX_LEVEL){
		redLevel += theRed;
		ok = 1;
	}
	pthread_mutex_unlock(&redLock);
	return ok;
}

int refillGreenInk(int theGreen){
	int ok = 0;
	pthread_mutex_lock(&greenLock);
	if (greenLevel + theGreen <= MAX_LEVEL){
		greenLevel += theGreen;
		ok = 1;
	}
	pthread_mutex_unlock(&greenLock);
	return ok;
}

int refillBlueInk(int theBlue){
	int ok = 0;
	pthread_mutex_lock(&blueLock);
	if (blueLevel + theBlue <= MAX_LEVEL){
		blueLevel += theBlue;
		ok = 1;
	}
	pthread_mutex_unlock(&blueLock);
	return ok;
}

//------------------------------------------------------------------------
//	You shouldn't have to touch this one.  Definitely if you don't
//	add the "producer" threads, and probably not even if you do.
//------------------------------------------------------------------------
void speedupProducers(void){
	//	decrease sleep time by 20%, but don't get too small
	int newSleepTime = (8 * producerSleepTime) / 10;
	
	if (newSleepTime > MIN_SLEEP_TIME)
	{
		producerSleepTime = newSleepTime;
	}

	printf("%d\n", producerSleepTime);
}

void slowdownProducers(void){
	//	increase sleep time by 20%
	producerSleepTime = (12 * producerSleepTime) / 10;
	printf("%d\n", producerSleepTime);
}

//------------------------------------------------------------------------
//	You shouldn't have to change anything in the main function
//------------------------------------------------------------------------
int main(int argc, char** argv){
	initializeFrontEnd(argc, argv, displayGridPane, displayStatePane);
	
	//	Now we can do application-level
	initializeApplication();

	//	Now we enter the main loop of the program and to a large extend
	//	"lose control" over its execution.  The callback functions that 
	//	we set up earlier will be called when the corresponding event
	//	occurs
	glutMainLoop();
	
	//	Free allocated resource before leaving (not absolutely needed, but
	//	just nicer.  Also, if you crash there, you know something is wrong
	//	in your code.
	for (int i=0; i< NUM_ROWS; i++)
		free(grid[i]);
	free(grid);
	//
	free(travelList);

	delete travelersList;
	delete producersList;
	
	//	This will never be executed (the exit point will be in one of the
	//	call back functions).
	return 0;
}


//==================================================================================
//
//	This is a part that you have to edit and add to.
//
//==================================================================================

bool isInCorner(int row, int col){
	int farLeft = 0;
	int farRight = 19;

	int top = 0;
	int bottom = 19;

	if(col == farLeft && row == top) return true;
	else if(col == farLeft && row == bottom) return true;
	else if(col == farRight && row == top) return true;
	else if(col == farRight && row == bottom) return true;
	return false;
}

int lockAndAcquireInk(TravelerType type){
	int ink = 0;
	while(ink == 0){
		if(type == RED_TRAV){
			ink = acquireRedInk(1);
		}else if(type == GREEN_TRAV){
			ink = acquireGreenInk(1);
		}else if(type == BLUE_TRAV){
			ink = acquireBlueInk(1);
		}
	}
	return ink;
}

void writeTrail(int row, int col, TravelerType type){
	pthread_mutex_lock(&travelerGridLock);
	if(type == RED_TRAV){
        int addPart = grid[row][col] & 0x000000FF;
        int rest = grid[row][col] & 0xFFFFFF00;

        addPart += 0x20;
        if(addPart > 0xFF) addPart = 0xFF;

        grid[row][col] = rest | addPart;
    }else if(type == GREEN_TRAV){
        int addPart = grid[row][col] & 0x0000FF00;
        int rest = grid[row][col] & 0xFFFF00FF;

        addPart += 0x2000;
        if(addPart > 0xFF00) addPart = 0xFF00;

        grid[row][col] = rest | addPart;
    }else{
        int addPart = grid[row][col] & 0x00FF0000;
        int rest = grid[row][col] & 0xFF00FFFF;

        addPart += 0x200000;
        if(addPart > 0xFF0000) addPart = 0xFF0000;

        grid[row][col] = rest | addPart;
    }
	pthread_mutex_unlock(&travelerGridLock);
}

void checkAndWriteNewDirection(int &travelDist, TravelerInfo *info){
	bool accepted = false;
	int maxDist = 14;
	int currDist = 0;
	TravelDirection currDir;

	while(! accepted){
		pthread_mutex_lock(&travelerListLock);
		currDir = info->dir;
		pthread_mutex_unlock(&travelerListLock);
		int randVal = rand() % 2;
		if(randVal == 0){
			currDir = (TravelDirection)((currDir + 1) % NUM_TRAVEL_DIRECTIONS);
		}else{
			currDir = (TravelDirection)((currDir + 3) % NUM_TRAVEL_DIRECTIONS);
		}

		currDist = (rand() % maxDist) + 1;

		switch(currDir){
			case NORTH:
				if(info->row + currDist < 20) accepted = true;
				break;
			case SOUTH:
				if(info->row - currDist >= 0) accepted = true;
				break;
			case EAST:
				if(info->col + currDist < 20) accepted = true;
				break;
			case WEST:
				if(info->col - currDist >= 0) accepted = true;
				break;
		}
		maxDist--;
	}

	travelDist = currDist;
	pthread_mutex_lock(&travelerListLock);
	info->dir = currDir;
	pthread_mutex_unlock(&travelerListLock);
}

void* travelerThreadFunction(void *threadInfo){
	TravelerInfo *info = (TravelerInfo *)(threadInfo);
	int travelDist;
	while(! isInCorner(info->row, info->col)){
		checkAndWriteNewDirection(travelDist, info);
		while(travelDist > 0){
			lockAndAcquireInk(info->type);
			writeTrail(info->row, info->col, info->type);
			switch(info->dir){
				case NORTH:
					info->row += 1;
					break;
				case SOUTH:
					info->row -= 1;
					break;
				case EAST:
					info->col += 1;
					break;
				case WEST:
					info->col -= 1;
					break;
				default:
					printf("Something went wrong with getting the direction of a thread\n");
					exit(1);
					break;
			}
			travelDist --;
			usleep(500000);
		}
	}

	pthread_mutex_lock(&travelerListLock);
	info->isLive = 0;
	pthread_mutex_unlock(&travelerListLock);

	pthread_mutex_lock(&threadNumLock);
	numLiveThreads --;
	pthread_mutex_unlock(&threadNumLock);

	return NULL;
}

void* producerThreadFunction(void *travelerType){
	TravelerType type = *(TravelerType*)(travelerType);
	if(type == RED_TRAV){
		while(true){
			usleep(producerSleepTime);
			refillRedInk(rand() % MAX_ADD_INK);
		}
	}else if(type == GREEN_TRAV){
		while(true){
			usleep(producerSleepTime);
			refillGreenInk(rand() % MAX_ADD_INK);
		}
	}else{
		while(true){
			usleep(producerSleepTime);
			refillBlueInk(rand() % MAX_ADD_INK);
		}
	}
}


void initializeApplication(void){
	//	Allocate the grid
	grid = (int**) malloc(NUM_ROWS * sizeof(int*));
	for (int i=0; i<NUM_ROWS; i++)
		grid[i] = (int*) malloc(NUM_COLS * sizeof(int));
	
	//---------------------------------------------------------------
	//	All the code below to be replaced/removed
	//	I initialize the grid's pixels to have something to look at
	//---------------------------------------------------------------
	//	Yes, I am using the C random generator after ranting in class that the C random
	//	generator was junk.  Here I am not using it to produce "serious" data (as in a
	//	simulation), only some color, in meant-to-be-thrown-away code
	
	//	seed the pseudo-random generator
	
	srand((unsigned int) time(NULL));
	
	//	create RGB values (and alpha  = 255) for each pixel
	for (int i=0; i<NUM_ROWS; i++){
		for (int j=0; j<NUM_COLS; j++){
			grid[i][j] = 0xFF000000;
		}
	}

	pthread_mutex_init(&travelerGridLock, NULL);
	pthread_mutex_init(&travelerListLock, NULL);
	
	pthread_mutex_init(&threadNumLock, NULL);

	pthread_mutex_init(&redLock, NULL);
	pthread_mutex_init(&greenLock, NULL);
	pthread_mutex_init(&blueLock, NULL);

	//	Enable this code if you want to do the traveler information
	//	maintaining extra credit section
	travelList = (TravelerInfo*) malloc(MAX_NUM_TRAVELER_THREADS * sizeof(TravelerInfo));
	for (int k=0; k< MAX_NUM_TRAVELER_THREADS; k++){
		travelList[k].type = (TravelerType)(rand() % NUM_TRAV_TYPES);
		travelList[k].row = rand() % NUM_ROWS;
		travelList[k].col = rand() % NUM_COLS;
		travelList[k].dir = (TravelDirection)(rand() % NUM_TRAVEL_DIRECTIONS);
		travelList[k].isLive = 1;
		
	}

	for(int i = 0; i < MAX_NUM_TRAVELER_THREADS; i++){
		pthread_create(travelersList + i, NULL, travelerThreadFunction, travelList + i);
		pthread_mutex_lock(&threadNumLock);
		numLiveThreads ++;
		pthread_mutex_unlock(&threadNumLock);
	}

	TravelerType red = RED_TRAV;
	TravelerType green = GREEN_TRAV;
	TravelerType blue = BLUE_TRAV;

	for(int i = 0; i < 3; i++){
		pthread_create(producersList + i, NULL, producerThreadFunction, &red);
	}

	for(int i = 0; i < 3; i++){
		pthread_create(producersList + i + 3, NULL, producerThreadFunction, &green);
	}

	for(int i = 0; i < 3; i++){
		pthread_create(producersList + i + 6, NULL, producerThreadFunction, &blue);
	}
}



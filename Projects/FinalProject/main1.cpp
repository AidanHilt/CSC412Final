/**
 * @file main1.cpp
 * @author Aidan Hilt/Derek Jacobs
 * @brief This is version 1 of the final project
 * @version 0.1
 * @date 2019-12-23
 * 
 * @copyright Copyright (c) 2019
 * 
 */
//
//  main.c
//  Final Project CSC412
//
//  Created by Jean-Yves Hervé on 2019-12-12
//	This is public domain code.  By all means appropriate it and change is to your
//	heart's content.
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <unistd.h>
//
#include "gl_frontEnd.h"

using namespace std;

//==================================================================================
//	Function prototypes
//==================================================================================
void displayGridPane(void);
void displayStatePane(void);
void initializeApplication(void);
void cleanupGridAndLists(void);

//==================================================================================
//	Application-level global variables
//==================================================================================

const char* OUT_FILE_NAME = "robotSimulOut.txt";

//	Don't touch
extern int	GRID_PANE, STATE_PANE;
extern int 	GRID_PANE_WIDTH, GRID_PANE_HEIGHT;
extern int	gMainWindow, gSubwindow[2];

//	Don't rename any of these variables
//-------------------------------------
//	The state grid and its dimensions (arguments to the program)
int** grid;
int numRows = -1;	//	height of the grid
int numCols = -1;	//	width
int numBoxes = -1;	//	also the number of robots
int numDoors = -1;	//	The number of doors.

int numLiveThreads = 0;		//	the number of live robot threads

//	robot sleep time between moves (in microseconds)
const int MIN_SLEEP_TIME = 1000;
int robotSleepTime = 100000;

//	An array of C-string where you can store things you want displayed
//	in the state pane to display (for debugging purposes?)
//	Dont change the dimensions as this may break the front end
const int MAX_NUM_MESSAGES = 8;
const int MAX_LENGTH_MESSAGE = 32;
char** message;
time_t startTime;

//An array storing the locations 
//of the robots. Is set to {-1, -1}
//when the robot is done
int **robotLocations;

//An array storing the locations
//of the boxes
int **boxLocations;

//An array that corresponds each
//door and box to its assigned door
int *doorAssignments;

//An array of arrays storing
//the locations of the doors
int **doorLocations;

//==================================================================================
//	These are the functions that tie the simulation with the rendering.
//	Some parts are "don't touch."  Other parts need your intervention
//	to make sure that access to critical section is properly synchronized
//==================================================================================


void displayGridPane(void)
{
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[GRID_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, GRID_PANE_HEIGHT, 0);
	glScalef(1.f, -1.f, 1.f);
	
	//-----------------------------
	//	CHANGE THIS
	//-----------------------------
	//	Here I hard-code myself some data for robots and doors.  Obviously this code
	//	this code must go away.  I just want to show you how to display the information
	//	about a robot-box pair or a door.
	//	Important here:  I don't think of the locations (robot/box/door) as x and y, but
	//	as row and column.  So, the first index is a row (y) coordinate, and the second
	//	index is a column (x) coordinate.
	for (int i=0; i<numBoxes; i++){
		if (robotLocations[i][0] != -1 && robotLocations[i][0] != -1)
			drawRobotAndBox(i, robotLocations[i][0], robotLocations[i][1], boxLocations[i][0], boxLocations[i][1], doorAssignments[i]);
	}

	for (int i=0; i<numDoors; i++){
		//	here I would test if the robot thread is still alive
		//				row				column	
		drawDoor(i, doorLocations[i][0], doorLocations[i][1]);
	}

	//	This call does nothing important. It only draws lines
	//	There is nothing to synchronize here
	drawGrid();

	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

/**
 * This function will generate a random starting coordinate for each of the doors,boxes, robots
 * @param type, what object the positions are for
 * @remark 0 type is a traveler, 1 is a box, 2 is a door
 * @remark this function checks to make sure a square is unoccupied before it places an object
 * @return an int * containing the starting row and column of the object
 **/
int* generateStartPos(int type);

/**
 * @brief This function generates a door assignment for each robot
 * @return nothing
 */
void generateDoorAssignments(void);

/**
 * @brief this function finds the vertical position from which the traveler will need to push the box from
 * @param boxPos, the position of the box
 * @param doorPos, the position of the door
 * @return int* the coordinate pair for the position the traveler needs to reach
 */
int *findPushPosVert(int *boxPos, int* doorPos);

/**
 * @brief this function finds the horizontal position from which the traveler will need to push the box from
 * @param boxPos, the position of the box
 * @param doorPos, the position of the door
 * @return int* the coordinate pair for the position the traveler needs to reach
 */
int *findPushPosHor(int *boxPos, int* doorPos);

/**
 * @brief Moves a robot 1 position, in the direction
 * specified. It will check if its planned destination
 * is valid, and wait until otherwise
 * 
 * @param robotPosition The current position of the robot
 * @param dir The direction that the robot should move in
 */
void move(int *robotPosition, Direction dir, int index);

/**
 * @brief Moves a robot and a box 1 position,
 * in the direction specified
 * 
 * @param robotPosition The current position of the robot
 * @param boxPosition The current position of the box
 * @param dir The direction the robot and box
 * should move in
 */
void push(int *robotPosition, int* boxPosition, Direction dir, int index);

/**
 * @brief This function will move the traveler to where it needs to be
 * to push a box
 * 
 * @param position, the position the robot must move to
 * @param robotIndex the index of the robot that will be moving
 */
void moveToDestination(int robotIndex, int *position);

/**
 * @brief This function will push a box to a given destination
 * 
 *@param robotIndex, which robot will be moving
 * @param position , the detination position
 */
void pushToDestination(int robotIndex, int *position);

void displayStatePane(void){
	//	This is OpenGL/glut magic.  Don't touch
	glutSetWindow(gSubwindow[STATE_PANE]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//	Here I hard-code a few messages that I want to see displayed
	//	in my state pane.  The number of live robot threads will
	//	always get displayed.  No need to pass a message about it.
	time_t currentTime = time(NULL);
	double deltaT = difftime(currentTime, startTime);

	int numMessages = 3;
	sprintf(message[0], "We have %d doors", numDoors);
	sprintf(message[1], "I like cheese");
	sprintf(message[2], "Run time is %4.0f", deltaT);

	//---------------------------------------------------------
	//	This is the call that makes OpenGL render information
	//	about the state of the simulation.
	//
	//	You *must* synchronize this call.
	//
	//---------------------------------------------------------
	drawState(numMessages, message);
	
	
	//	This is OpenGL/glut magic.  Don't touch
	glutSwapBuffers();
	
	glutSetWindow(gMainWindow);
}

//------------------------------------------------------------------------
//	You shouldn't have to touch this one.  Definitely if you don't
//	add the "producer" threads, and probably not even if you do.
//------------------------------------------------------------------------
void speedupRobots(void){
	//	decrease sleep time by 20%, but don't get too small
	int newSleepTime = (8 * robotSleepTime) / 10;
	
	if (newSleepTime > MIN_SLEEP_TIME)
	{
		robotSleepTime = newSleepTime;
	}
}

void slowdownRobots(void){
	//	increase sleep time by 20%
	robotSleepTime = (12 * robotSleepTime) / 10;
}


//------------------------------------------------------------------------
//	You shouldn't have to change anything in the main function besides
//	the initialization of numRows, numCos, numDoors, numBoxes.
//------------------------------------------------------------------------
int main(int argc, char** argv){
	//	We know that the arguments  of the program  are going
	//	to be the width (number of columns) and height (number of rows) of the
	//	grid, the number of boxes (and robots), and the number of doors.
	//	You are going to have to extract these.  For the time being,
	//	I hard code-some values
	if(argc != 5){
		printf("Incorrect number of arguments. You must provide the number of rows,\n the number of columns, a number between one\nand three to represent the number of doors, and the number of robots/boxes\n");
		exit(1);
	}
	numRows = atoi(argv[1]);
	numCols = atoi(argv[2]);
	numBoxes = atoi(argv[3]);
	numDoors = atoi(argv[4]);
	
	if(numDoors < 1 || numDoors > 3){
		printf("There must be between 1 and 3 doors");
		exit(1);
	}

	//	Even though we extracted the relevant information from the argument
	//	list, I still need to pass argc and argv to the front-end init
	//	function because that function passes them to glutInit, the required call
	//	to the initialization of the glut library.
	initializeFrontEnd(argc, argv, displayGridPane, displayStatePane);
	
	//	Now we can do application-level initialization
	initializeApplication();

	//	Now we enter the main loop of the program and to a large extend
	//	"lose control" over its execution.  The callback functions that 
	//	we set up earlier will be called when the corresponding event
	//	occurs
	glutMainLoop();
	
	cleanupGridAndLists();
	
	//	This will probably never be executed (the exit point will be in one of the
	//	call back functions).
	return 0;
}

//---------------------------------------------------------------------
//	Free allocated resource before leaving (not absolutely needed, but
//	just nicer.  Also, if you crash there, you know something is wrong
//	in your code.
//---------------------------------------------------------------------
void cleanupGridAndLists(void){
	for (int i=0; i< numRows; i++)
		free(grid[i]);
	free(grid);
	for (int k=0; k<MAX_NUM_MESSAGES; k++)
		free(message[k]);
	free(message);
}


//==================================================================================
//
//	This is a part that you have to edit and add to.
//
//==================================================================================

void* threadFunction(void * idx){
	int index = *(int*)(idx);

	int *horizontalPushLocation = findPushPosHor(boxLocations[index], doorLocations[doorAssignments[index]]);
	if(horizontalPushLocation[1] != -1)
	{
		moveToDestination(index, horizontalPushLocation);
	}

	int pushDestination[2] = {horizontalPushLocation[0], doorLocations[doorAssignments[index]][1]};
	pushToDestination(index, pushDestination);

	int *verticalPushLocation = findPushPosVert(boxLocations[index], doorLocations[doorAssignments[index]]);
	if(verticalPushLocation[0] != -1)
	{
		moveToDestination(index, verticalPushLocation);
	}

	int vertPushDestination[2] = {doorLocations[doorAssignments[index]][0], verticalPushLocation[1]};
	pushToDestination(index, vertPushDestination);
	
	ofstream out;
	out.open(OUT_FILE_NAME, ios::app);
	out << "Robot " << index << " end\n";
	out.close();

	robotLocations[index][0] = -1;
	robotLocations[index][1] = -1;

	return NULL;
}

int *findPushPosVert(int *boxPos, int* doorPos)
{
	int *pos = new int[2];
	pos[1] = boxPos[1];
	if(boxPos[0] > doorPos[0])
	{
		pos[0] = boxPos[0] + 1;
	}
	else if (boxPos[0] < doorPos[0])
	{
		pos[0] = boxPos[0] - 1;
	}
	else
	{
		pos[0] = -1;
	}

	return pos;
}

int *findPushPosHor(int *boxPos, int* doorPos)
{
	int *pos = new int[2];
	pos[0] = boxPos[0];
	if(boxPos[1] > doorPos[1])
	{
		pos[1] = boxPos[1] + 1;
	}
	else if (boxPos[1] < doorPos[1])
	{
		pos[1] = boxPos[1] - 1;
	}
	else
	{
		pos[1] = -1;
	}	

	return pos;
}

void initializeApplication(void){
	//	Allocate the grid
	grid = (int**) malloc(numRows * sizeof(int*));
	for (int i=0; i<numRows; i++)
		grid[i] = (int*) malloc(numCols * sizeof(int));
	
	message = (char**) malloc(MAX_NUM_MESSAGES*sizeof(char*));
	for (int k=0; k<MAX_NUM_MESSAGES; k++)
		message[k] = (char*) malloc((MAX_LENGTH_MESSAGE+1)*sizeof(char));
		
	//---------------------------------------------------------------
	//	All the code below to be replaced/removed
	//	I initialize the grid's pixels to have something to look at
	//---------------------------------------------------------------
	//	Yes, I am using the C random generator after ranting in class that the C random
	//	generator was junk.  Here I am not using it to produce "serious" data (as in a
	//	simulation), only some color, in meant-to-be-thrown-away code
	
	//	seed the pseudo-random generator
	startTime = time(NULL);
	srand((unsigned int) startTime);

	ofstream logging;
	
	logging.open(OUT_FILE_NAME, ios::trunc);
	logging << numRows << " " << numCols << " " << numBoxes << " " << numDoors << "\n\n";


	doorLocations = new int*[numDoors];
	
	for(int i = 0; i < numDoors; i++){
		doorLocations[i] = new int[2];

		int *coord = generateStartPos(2);

		doorLocations[i][0] = coord[0];
		doorLocations[i][1] = coord[1];

		grid[coord[0]][coord[1]] = 1;

		logging << coord[0] << " " << coord[1] << '\n';

		delete coord;
	}

	logging << '\n';

	
	robotLocations = new int*[numBoxes];
	boxLocations = new int*[numBoxes];

	for(int i = 0; i < numBoxes; i++){
		boxLocations[i] = new int[2];

		int *coord = generateStartPos(1);

		boxLocations[i][0] = coord[0];
		boxLocations[i][1] = coord[1];

		grid[coord[0]][coord[1]] = 1;

		logging << coord[0] << " " << coord[1] << '\n';

		delete coord;
	}
	
	logging << '\n';
	
	for(int i = 0; i < numBoxes; i++){
		robotLocations[i] = new int[2];

		int *coord = generateStartPos(0);

		robotLocations[i][0] = coord[0];
		robotLocations[i][1] = coord[1];
		
		grid[coord[0]][coord[1]] = 1;
		
		logging << coord[0] << " " << coord[1] << '\n';
		
		delete coord;
	}

	logging << '\n';

	generateDoorAssignments();

	logging.close();

	for(int i = 0; i < numBoxes; i++)
	{
		int *arg = &i;

		threadFunction((void *)arg);
	}
}

int* generateStartPos(int type){
	//Style is [row, col]
	int* coord = new int[2];
	int row = 0;
	int col = 0;
	//Used for box positions
	int min = 1;
	int rowMax = numRows - 2;
	int colMax = numCols - 2;

	
	switch(type)
	{
		case 0:
		case 2:
			//Traveler and door can be placed anywhere
			row = (rand() % numRows);
			col = (rand() % numCols);
			while(grid[row][col] == 1)
			{
				row = (rand() % numRows);
				col = (rand() % numCols);
			}
			break;
		case 1:
			//Box, cannot be on an edge
			row = (rand() % (rowMax - min + 1) + min);
			col = (rand() % (colMax - min + 1) + min);
			while(grid[row][col] == 1)
			{
				row = (rand() % (rowMax - min + 1) + min);
				col = (rand() % (colMax - min + 1) + min);	
			}
			break;
		default:
			//Should not reach this point
			std::cout << "Something went wrong" << std::endl;
			return 0;
			break;
	}

	coord[0] = row;
	coord[1] = col;

	return coord;
}

void generateDoorAssignments(void){

	doorAssignments = new int[numBoxes];
	for(int i = 0; i < numBoxes; ++i){
		doorAssignments[i] = rand() % numDoors;
	}
	return;
}

void move(int *robotPosition, Direction dir, int index){
	ofstream logging;
	logging.open(OUT_FILE_NAME, ios::app);
	switch(dir){
		case 0:
			robotPosition[0] += 1;
			logging << "Robot " << index << " moved N\n";
			break;
		case 1:
			robotPosition[1] -= 1;
			logging << "Robot " << index << " moved W\n";
			break;
		case 2:
			robotPosition[0] -= 1;
			logging << "Robot " << index << " moved S\n";
			break;
		case 3:
			robotPosition[1] += 1;
			logging << "Robot " << index << " moved E\n";
			break;
		default:
			logging << "An invalid direction to move was received and the program aborted\n";
			exit(-1);
			break;
	}

	logging.close();
}

void push(int *robotPosition, int *boxPosition, Direction dir, int index){
	ofstream logging;
	logging.open(OUT_FILE_NAME, ios::app);
	switch(dir){
		case 0:
			robotPosition[0] += 1;
			boxPosition[0] += 1;
			logging << "Robot " << index << " pushed N\n";
			break;
		case 1:
			robotPosition[1] -= 1;
			boxPosition[1] -= 1;
			logging << "Robot " << index << " pushed W\n";
			break;
		case 2:
			robotPosition[0] -= 1;
			boxPosition[0] -= 1;
			logging << "Robot " << index << " pushed S\n";
			break;
		case 3:
			robotPosition[1] += 1;
			boxPosition[1] += 1;
			logging << "Robot " << index << " pushed E\n";
			break;
		default:
			logging << "An invalid direction to push was received and the program aborted\n";
			exit(-1);
			break;
	}

	logging.close();
}

void moveToDestination(int robotIndex, int *position)
{
	//First, move to the vertical position
	while(robotLocations[robotIndex][0] != position[0])
	{
		if(robotLocations[robotIndex][0] > position[0])
		{
			move(robotLocations[robotIndex], SOUTH, robotIndex);
		}
		if(robotLocations[robotIndex][0] < position[0])
		{
			move(robotLocations[robotIndex], NORTH, robotIndex);
		}

		displayGridPane();
		displayStatePane();

		usleep(robotSleepTime);			
	}

	//Then, move to the horizontal position
	while(robotLocations[robotIndex][1] != position[1])
	{
		if(robotLocations[robotIndex][1] > position[1])
		{
			move(robotLocations[robotIndex], WEST, robotIndex);
		}
		if(robotLocations[robotIndex][1] < position[1])
		{
			move(robotLocations[robotIndex], EAST, robotIndex);
		}
		
		displayGridPane();
		displayStatePane();

		usleep(robotSleepTime);
	}
}

void pushToDestination(int robotIndex, int *position)
{
	//First, move the box horizontally if need be
	while(boxLocations[robotIndex][1] != position[1])
	{
		if(boxLocations[robotIndex][1] > position[1])
		{
			push(robotLocations[robotIndex], boxLocations[robotIndex], WEST, robotIndex);
		}
		if(boxLocations[robotIndex][1] < position[1])
		{
			push(robotLocations[robotIndex], boxLocations[robotIndex], EAST, robotIndex);
		}

		displayGridPane();
		displayStatePane();

		usleep(robotSleepTime);
	}

	//Then vertically if we needed to
	while(boxLocations[robotIndex][0] != position[0])
	{
		if(boxLocations[robotIndex][0] > position[0])
		{
			push(robotLocations[robotIndex], boxLocations[robotIndex], SOUTH, robotIndex);
		}
		if(boxLocations[robotIndex][0] < position[0])
		{
			push(robotLocations[robotIndex], boxLocations[robotIndex], NORTH, robotIndex);
		}

		displayGridPane();
		displayStatePane();

		usleep(robotSleepTime);
	}
}
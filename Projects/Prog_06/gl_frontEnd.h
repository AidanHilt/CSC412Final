//
//  gl_frontEnd.h
//  GL threads
//
//  Created by Jean-Yves Hervé on 2017-04-24.
//  Copyright © 2017 Jean-Yves Hervé. All rights reserved.
//

#ifndef GL_FRONT_END_H
#define GL_FRONT_END_H


//------------------------------------------------------------------------------
//	Find out whether we are on Linux or macOS (sorry, Windows people)
//	and load the OpenGL & glut headers.
//	For the macOS, lets us choose which glut to use
//------------------------------------------------------------------------------
#if (defined(__dest_os) && (__dest_os == __mac_os )) || \
	defined(__APPLE_CPP__) || defined(__APPLE_CC__)
	//	Either use the Apple-provided---but deprecated---glut
	//	or the third-party freeglut install
	#if 1
		#include <GLUT/GLUT.h>
	#else
		#include <GL/freeglut.h>
		#include <GL/gl.h>
	#endif
#elif defined(linux)
	#include <GL/glut.h>
#else
	#error unknown OS
#endif


//-----------------------------------------------------------------------------
//	Data types
//-----------------------------------------------------------------------------

//	Travel direction data type
//	Note that if you define a variable
//	TravelDirection dir = whatever;
//	you get the opposite directions from dir as (NUM_TRAVEL_DIRECTIONS - dir)
//	you get left turn from dir as (dir + 1) % NUM_TRAVEL_DIRECTIONS
typedef enum TravelDirection {
	NORTH = 0,
	WEST,
	SOUTH,
	EAST,
	//
	NUM_TRAVEL_DIRECTIONS
} TravelDirection;

//	The 
typedef enum TravelerType {
	RED_TRAV = 0,
	GREEN_TRAV,
	BLUE_TRAV,
	//
	NUM_TRAV_TYPES
} TravelerType;

//	Traveler info data type
typedef struct TravelerInfo {
	TravelerType type;
	//	location of the traveler
	int row;
	int col;
	//	in which direciton is the traveler going
	TravelDirection dir;
	// initialized to 1, set to 0 if terminates
	int isLive;
} TravelerInfo;


//-----------------------------------------------------------------------------
//	Function prototypes
//-----------------------------------------------------------------------------

void drawGrid(int**grid, int numRows, int numCols);
void drawGridAndTravelers(int**grid, int numRows, int numCols, TravelerInfo *travelList);
void drawState(int numLiveThreads, int redLevel, int greenLevel, int blueLevel);
void initializeFrontEnd(int argc, char** argv, void (*gridCB)(void), void (*stateCB)(void));

#endif // GL_FRONT_END_H


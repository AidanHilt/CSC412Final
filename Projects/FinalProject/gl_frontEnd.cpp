//
//  gl_frontEnd.c
//  GL threads
//
//  Created by Jean-Yves Hervé on 2018-12-05

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
 +-------------------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//
#include "gl_frontEnd.h"


extern int MAX_NUM_MESSAGES;
extern int MAX_LENGTH_MESSAGE;

extern int** grid;
extern int numRows;
extern int numCols;
extern int numBoxes;
extern int numDoors;

extern int numLiveThreads;


//	Defined in main.c
void speedupRobots(void);
void slowdownRobots(void);


//---------------------------------------------------------------------------
//  Private functions' prototypes
//---------------------------------------------------------------------------

void myResize(int w, int h);
void drawnTankFrame(int LEVEL_WIDTH, int LEVEL_HEIGHT);
void fillTank(int y, int LEVEL_WIDTH);
void displayTextualInfo(const char* infoStr, int x, int y, int isLarge);
void myMouse(int b, int s, int x, int y);
void myGridPaneMouse(int b, int s, int x, int y);
void myStatePaneMouse(int b, int s, int x, int y);
void myKeyboard(unsigned char c, int x, int y);
void myTimerFunc(int val);
void createDoorColors(void);
void freeDoorColors(void);

//---------------------------------------------------------------------------
//  Interface constants
//---------------------------------------------------------------------------


#define SMALL_DISPLAY_FONT    GLUT_BITMAP_HELVETICA_10
#define MEDIUM_DISPLAY_FONT   GLUT_BITMAP_HELVETICA_12
#define LARGE_DISPLAY_FONT    GLUT_BITMAP_HELVETICA_18
const int SMALL_FONT_HEIGHT = 12;
const int LARGE_FONT_HEIGHT = 18;
const int TEXT_PADDING = 0;
const float kTextColor[4] = {1.f, 1.f, 1.f, 1.f};

const int   INIT_WIN_X = 100,
            INIT_WIN_Y = 40;

int GRID_PANE_WIDTH = 805;
int GRID_PANE_HEIGHT = 605;
int STATE_PANE_WIDTH = 400;
int STATE_PANE_HEIGHT = 600;
const int H_PADDING = 0;
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 600;


//---------------------------------------------------------------------------
//  File-level global variables
//---------------------------------------------------------------------------

void (*gridDisplayFunc)(void);
void (*stateDisplayFunc)(void);

//	We use a window split into two panes/subwindows.  The subwindows
//	will be accessed by an index.
int	GRID_PANE = 0,
	STATE_PANE = 1;
int	gMainWindow,
	gSubwindow[2];

float** doorColor;


//---------------------------------------------------------------------------
//	Drawing functions
//---------------------------------------------------------------------------

void drawRobotAndBox(int id,
					 int robotRow, int robotCol,
					 int boxRow, int boxCol,
					 int doorNumber)
{
	//	Yes, I know that it's inefficient to recompute this each and every time,
	//	but gcc on Ubuntu doesn't let me define these as static [!??]
	const int	DH = GRID_PANE_WIDTH / numCols,
				DV = GRID_PANE_HEIGHT / numRows;

	//	my boxes are brown with a contour the same color as destination door
	glColor4f(0.25f, 0.25f, 0.f, 1.f);
	glPushMatrix();
	glTranslatef((boxCol+ 0.125f)*DH, (boxRow+0.125f)*DV, 0.f);
	glScalef(0.75f, 0.75f, 1.f);
	glBegin(GL_POLYGON);
		glVertex2i(0, 0);
		glVertex2i(DH, 0);
		glVertex2i(DH, DV);
		glVertex2i(0, DV);
	glEnd();
	glColor4fv(doorColor[doorNumber]);
	glBegin(GL_LINE_LOOP);
		glVertex2i(0, 0);
		glVertex2i(DH, 0);
		glVertex2i(DH, DV);
		glVertex2i(0, DV);
	glEnd();
	glPopMatrix();
	char boxStr[4];
	sprintf(boxStr, "B%d", id);
	displayTextualInfo(boxStr, (boxCol+0.25f)*DH,(boxRow+0.66f)*DV, 0);

	//	my robots are dark gray with a contour the same color as destination door
	glColor4f(0.25f, 0.25f, 0.f, 1.f);
	glPushMatrix();
	glTranslatef((robotCol+0.5f)*DH, (robotRow+0.125f)*DV, 0.f);
	glScalef(0.375f, 0.375f, 31.f);
	glBegin(GL_POLYGON);
		glVertex2i(0, 0);
		glVertex2i(DH, DV);
		glVertex2i(0, 2*DV);
		glVertex2i(-DH, DV);
	glEnd();
	glColor4fv(doorColor[doorNumber]);
	glBegin(GL_LINE_LOOP);
		glVertex2i(0, 0);
		glVertex2i(DH, DV);
		glVertex2i(0, 2*DV);
		glVertex2i(-DH, DV);
	glEnd();
	glPopMatrix();
	char robotStr[4];
	sprintf(robotStr, "R%d", id);
	displayTextualInfo(robotStr, (robotCol+0.25f)*DH, (robotRow+0.66f)*DV, 0);


}

//	This function assigns a color to the door based on its number
void drawDoor(int doorNumber, int doorRow, int doorCol)
{
	//	Yes, I know that it's inefficient to recompute this each and every time,
	//	but gcc on Ubuntu doesn't let me define these as static [!??]
	const int	DH = GRID_PANE_WIDTH / numCols,
				DV = GRID_PANE_HEIGHT / numRows;

	glColor4fv(doorColor[doorNumber]);
	glBegin(GL_POLYGON);
		glVertex2i(doorCol*DH, doorRow*DV);
		glVertex2i((doorCol+1)*DH, doorRow*DV);
		glVertex2i((doorCol+1)*DH, (doorRow+1)*DV);
		glVertex2i(doorCol*DH, (doorRow+1)*DV);
	glEnd();

	char doorStr[4];
	sprintf(doorStr, "D%d", doorNumber);
	displayTextualInfo(doorStr, (doorCol+0.25f)*DH, (doorRow+0.66f)*DV, 0);
}

//	This is the function that does the actual grid drawing
void drawGrid(void)
{
	const int	DH = GRID_PANE_WIDTH / numCols,
				DV = GRID_PANE_HEIGHT / numRows;

	//	Then draw a grid of lines on top of the squares
	glColor4f(0.5f, 0.5f, 0.5f, 1.f);
	glBegin(GL_LINES);
		//	Horizontal
		for (int i=0; i<= numRows; i++)
		{
			glVertex2i(0, i*DV);
			glVertex2i(numCols*DH, i*DV);
		}
		//	Vertical
		for (int j=0; j<= numCols; j++)
		{
			glVertex2i(j*DH, 0);
			glVertex2i(j*DH, numRows*DV);
		}
	glEnd();
}



void displayTextualInfo(const char* infoStr, int xPos, int yPos, int fontSize)
{
    //-----------------------------------------------
    //  0.  get current material properties
    //-----------------------------------------------
    float oldAmb[4], oldDif[4], oldSpec[4], oldShiny;
    glGetMaterialfv(GL_FRONT, GL_AMBIENT, oldAmb);
    glGetMaterialfv(GL_FRONT, GL_DIFFUSE, oldDif);
    glGetMaterialfv(GL_FRONT, GL_SPECULAR, oldSpec);
    glGetMaterialfv(GL_FRONT, GL_SHININESS, &oldShiny);

    glPushMatrix();

    //-----------------------------------------------
    //  1.  Build the string to display <-- parameter
    //-----------------------------------------------
    int infoLn = (int) strlen(infoStr);

    //-----------------------------------------------
    //  2.  Determine the string's length (in pixels)
    //-----------------------------------------------
    int textWidth = 0;
	switch(fontSize)
	{
		case 0:
			for (int k=0; k<infoLn; k++)
			{
				textWidth += glutBitmapWidth(SMALL_DISPLAY_FONT, infoStr[k]);
			}
			break;

		case 1:
			for (int k=0; k<infoLn; k++)
			{
				textWidth += glutBitmapWidth(MEDIUM_DISPLAY_FONT, infoStr[k]);
			}
			break;

		case 2:
			for (int k=0; k<infoLn; k++)
			{
				textWidth += glutBitmapWidth(LARGE_DISPLAY_FONT, infoStr[k]);
			}
			break;

		default:
			break;
	}

	//  add a few pixels of padding
    textWidth += 2*TEXT_PADDING;

    //-----------------------------------------------
    //  4.  Draw the string
    //-----------------------------------------------
    glColor4fv(kTextColor);
    int x = xPos;
	switch(fontSize)
	{
		case 0:
			for (int k=0; k<infoLn; k++)
			{
				glRasterPos2i(x, yPos);
				glutBitmapCharacter(SMALL_DISPLAY_FONT, infoStr[k]);
				x += glutBitmapWidth(SMALL_DISPLAY_FONT, infoStr[k]);
			}
			break;

		case 1:
			for (int k=0; k<infoLn; k++)
			{
				glRasterPos2i(x, yPos);
				glutBitmapCharacter(MEDIUM_DISPLAY_FONT, infoStr[k]);
				x += glutBitmapWidth(MEDIUM_DISPLAY_FONT, infoStr[k]);
			}
			break;

		case 2:
			for (int k=0; k<infoLn; k++)
			{
				glRasterPos2i(x, yPos);
				glutBitmapCharacter(LARGE_DISPLAY_FONT, infoStr[k]);
				x += glutBitmapWidth(LARGE_DISPLAY_FONT, infoStr[k]);
			}
			break;

		default:
			break;
	}


    //-----------------------------------------------
    //  5.  Restore old material properties
    //-----------------------------------------------
	glMaterialfv(GL_FRONT, GL_AMBIENT, oldAmb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, oldDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR, oldSpec);
	glMaterialf(GL_FRONT, GL_SHININESS, oldShiny);

    //-----------------------------------------------
    //  6.  Restore reference frame
    //-----------------------------------------------
    glPopMatrix();
}



void drawState(int numMessages, char** message)
{
	//	I compute once the dimensions for all the rendering of my state info
	//	One other place to rant about that desperately lame gcc compiler.  It's
	//	positively disgusting that the code below is rejected.
	int LEFT_MARGIN = STATE_PANE_WIDTH / 12;
	int V_PAD = STATE_PANE_HEIGHT / 12;

	for (int k=0; k<numMessages; k++)
	{
		displayTextualInfo(message[k], LEFT_MARGIN, 3*STATE_PANE_HEIGHT/4 - k*V_PAD, 1);
	}

	//	display info about number of live threads
	char infoStr[256];
	sprintf(infoStr, "Live Threads: %d", numLiveThreads);
	displayTextualInfo(infoStr, LEFT_MARGIN, 7*STATE_PANE_HEIGHT/8, 2);
}


//	This callback function is called when the window is resized
//	(generally by the user of the application).
//	It is also called when the window is created, why I placed there the
//	code to set up the virtual camera for this application.
//
void myResize(int w, int h)
{
	if ((w != WINDOW_WIDTH) || (h != WINDOW_HEIGHT))
	{
		glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
	}
	else
	{
		glutPostRedisplay();
	}
}


void myDisplay(void)
{
    glutSetWindow(gMainWindow);

    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();

	gridDisplayFunc();
	stateDisplayFunc();

    glutSetWindow(gMainWindow);
}

//	This function is called when a mouse event occurs just in the tiny
//	space between the two subwindows.
//
void myMouse(int button, int state, int x, int y)
{
	glutSetWindow(gMainWindow);
	glutPostRedisplay();
}

//	This function is called when a mouse event occurs in the grid pane
//
void myGridPaneMouse(int button, int state, int x, int y)
{
	switch (button)
	{
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN)
			{
				//	do something
			}
			else if (state == GLUT_UP)
			{
				//	exit(0);
			}
			break;

		default:
			break;
	}

	glutSetWindow(gMainWindow);
	glutPostRedisplay();
}

//	This function is called when a mouse event occurs in the state pane
void myStatePaneMouse(int button, int state, int x, int y)
{
	switch (button)
	{
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN)
			{
				//	do something
			}
			else if (state == GLUT_UP)
			{
				//	exit(0);
			}
			break;

		default:
			break;
	}

	glutSetWindow(gMainWindow);
	glutPostRedisplay();
}


//	This callback function is called when a keyboard event occurs
//
void myKeyboard(unsigned char c, int x, int y)
{
	int ok = 0;

	switch (c)
	{
		//	'esc' to quit
		case 27:
			exit(0);
			break;

		//	slowdown
		case ',':
			slowdownRobots();
			break;

		//	speedup
		case '.':
			speedupRobots();
			break;

		default:
			ok = 1;
			break;
	}
	if (!ok)
	{
		//	do something?
	}

	glutSetWindow(gMainWindow);
	glutPostRedisplay();
}



void myTimerFunc(int value)
{
	//	value not used.  Warning suppression
	(void) value;

	glutTimerFunc(25, myTimerFunc, 0);

    myDisplay();
}



void initializeFrontEnd(int argc, char** argv, void (*gridDisplayCB)(void),
						void (*stateDisplayCB)(void))
{
	//	Initialize glut and create a new window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);


	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(INIT_WIN_X, INIT_WIN_Y);
	gMainWindow = glutCreateWindow("Colorful Trails -- CSC 412 - Spring 2017");
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);

	//	set up the callbacks for the main window
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myResize);
	glutMouseFunc(myMouse);
    glutTimerFunc(25, myTimerFunc, 0);

	gridDisplayFunc = gridDisplayCB;
	stateDisplayFunc = stateDisplayCB;

	//	create the two panes as glut subwindows
	gSubwindow[GRID_PANE] = glutCreateSubWindow(gMainWindow,
												0, 0,							//	origin
												GRID_PANE_WIDTH, GRID_PANE_HEIGHT);
    glViewport(0, 0, GRID_PANE_WIDTH, GRID_PANE_HEIGHT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0f, GRID_PANE_WIDTH, 0.0f, GRID_PANE_HEIGHT, -1, 1);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myGridPaneMouse);
	glutDisplayFunc(gridDisplayCB);


	glutSetWindow(gMainWindow);
	gSubwindow[STATE_PANE] = glutCreateSubWindow(gMainWindow,
												GRID_PANE_WIDTH + H_PADDING, 0,	//	origin
												STATE_PANE_WIDTH, STATE_PANE_HEIGHT);
    glViewport(0, 0, STATE_PANE_WIDTH, STATE_PANE_WIDTH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0.0f, STATE_PANE_WIDTH, 0.0f, STATE_PANE_HEIGHT, -1, 1);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myGridPaneMouse);
	glutDisplayFunc(stateDisplayCB);

	createDoorColors();
}

void createDoorColors(void)
{
	doorColor = (float**) malloc(numDoors * sizeof(float*));

	float hueStep = 360.f / numDoors;

	for (int k=0; k<numDoors; k++)
	{
		doorColor[k] = (float*) malloc(4*sizeof(float));

		//	compute a hue for the door
		float hue = k*hueStep;
		//	convert the hue to an RGB color
		int hueRegion = (int) (hue / 60);
		switch (hueRegion)
		{
				//  hue in [0, 60] -- red-green, dominant red
			case 0:
				doorColor[k][0] = 1.f;					//  red is max
				doorColor[k][1] = hue / 60.f;			//  green calculated
				doorColor[k][2] = 0.f;					//  blue is zero
				break;

				//  hue in [60, 120] -- red-green, dominant green
			case 1:
				doorColor[k][0] = (120.f - hue) / 60.f;	//  red is calculated
				doorColor[k][1] = 1.f;					//  green max
				doorColor[k][2] = 0.f;					//  blue is zero
				break;

				//  hue in [120, 180] -- green-blue, dominant green
			case 2:
				doorColor[k][0] = 0.f;					//  red is zero
				doorColor[k][1] = 1.f;					//  green max
				doorColor[k][2] = (hue - 120.f) / 60.f;	//  blue is calculated
				break;

				//  hue in [180, 240] -- green-blue, dominant blue
			case 3:
				doorColor[k][0] = 0.f;					//  red is zero
				doorColor[k][1] = (240.f - hue) / 60;	//  green calculated
				doorColor[k][2] = 1.f;					//  blue is max
				break;

				//  hue in [240, 300] -- blue-red, dominant blue
			case 4:
				doorColor[k][0] = (hue - 240.f) / 60;	//  red is calculated
				doorColor[k][1] = 0;						//  green is zero
				doorColor[k][2] = 1.f;					//  blue is max
				break;

				//  hue in [300, 360] -- blue-red, dominant red
			case 5:
				doorColor[k][0] = 1.f;					//  red is max
				doorColor[k][1] = 0;						//  green is zero
				doorColor[k][2] = (360.f - hue) / 60;	//  blue is calculated
				break;

			default:
				break;

		}
		doorColor[k][3] = 1.f;					//  alpha --> full opacity
	}
}

void freeDoorColors(void)
{
	for (int k=0; k<numDoors; k++)
		free(doorColor[k]);

	free(doorColor);
}

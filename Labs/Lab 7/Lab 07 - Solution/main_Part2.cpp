//
//  main.cpp
//  Lab 07 Solution - Part 2
//
//	This one actually implements a solution to the lab problem.
//	The parent process creates the pipes, then creates child processes.
//	It then starts reading data from the file and distributes it between
//	the child processes, via unnamed pipes.
//	This is a more realistic situation than one in which we read all the
//	data first:  In that case, there would be no need for pipes, since the
//	data could be sent through the argument list, as you are doing with the
//	dispatcher right now.
//
//	Anytime you use pipe communication, you need to do a bit of design of the
//	messages that will be sent.  Here I want to operate in the situation
//	where we don't know in advance how many data points each process will
//	receive.  So, the parent processes needs a way to inform the child process
//	that it has received the last data point.  So, I decide that I am going
//	to write my pipe messsages as human-readable strings.  Messsages can have
//	two forms
//		"DATA	<float number>  <int number>"     to send a data point
//		"END"	for the last message telling that no more data point is coming

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <cmath>
#include <cstring>
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


#if 0
//------------------------------------------
#pragma mark -
#pragma mark Global variables
//------------------------------------------
#endif

//	Now that we are in C++, use a const definition rather than #define
//	to define constants:  these constants will be visible in the
//	debugger.
const unsigned int NUM_CHILDREN = 4;
const unsigned int READ_END = 0;
const unsigned int WRITE_END = 1;

//	the parent process uses this array to store the pid of its child processes.
int childPid[NUM_CHILDREN];

//	I need to define my arrays of pipes out of the main function,
//	so that the "child function" has access to them
//		childToParent[child index][R/W];
//		parentToChild[child index][R/W];
int childToParent[NUM_CHILDREN][2];
int parentToChild[NUM_CHILDREN][2];

//	Alternatively, I could have used a 3D array
//		myPipe[child index][PtC/CtP][R/W]
//int myPipe[NUM_CHILDREN][2][2];

#if 0
//------------------------------------------
#pragma mark -
#pragma mark Function prototypes
//------------------------------------------
#endif

void childFunction(int index);


#if 0
//------------------------------------------
#pragma mark -
#pragma mark Implementation
//------------------------------------------
#endif

int main(int argc, const char * argv[])
{
	//--------------------------------------------------------------------------
	//	First, setup the pipes
	//--------------------------------------------------------------------------
	for (unsigned int i=0; i<NUM_CHILDREN; i++)
	{
		if (pipe(parentToChild[i]) == -1) {
			perror("Creation of pipe PtoC failed\n");
			exit(2);
		}
		if (pipe(childToParent[i]) == -1) {
			perror("Creation of pipe PtoC failed\n");
			exit(2);
		}
	}
	//--------------------------------------------------------------------------
	//	Next, create child processes
	//--------------------------------------------------------------------------
	for (unsigned int i=0; i<NUM_CHILDREN; i++)
	{
		childPid[i] = fork();
		if (childPid[i]==0)
		{
			//	The child process needs to know its index.
			//	The childFunction *must* end with an exit() call, so that the
			//	child does not come back to run this loop and create grandchildren
			childFunction(i);
		}
		//	parent
		else
		{
			//	close the end of the pipes that I don't need
			close(childToParent[i][WRITE_END]);
			close(parentToChild[i][READ_END]);
		}
	}
	
	//--------------------------------------------------------------------------
	//	Now, I start reading data and dispatching them to the child processes
	//--------------------------------------------------------------------------
	ifstream inFile;
	inFile.open("data.txt");
	int n;
	inFile >> n;

	for (int k=0; k<n; k++)
	{
		//	read a data point
		int m;
		float x;
		inFile >> x >> m;
		
		//	Format a data message, written into a buffer
		char message[BUFSIZ];
		sprintf(message, "DATA %f %d", x, m);
		
		//	sent the message to the approriate child
		unsigned int childIndex = k % NUM_CHILDREN;
		if (write(parentToChild[childIndex][WRITE_END], message, strlen(message)) == -1)
		{
			perror("Write on parentToChild failed");
			exit(5);
		}

		cout << "Parent sent message to Child " << childIndex << ":  " <<
				message << endl;
				
		//	Here, I sleep a bit between data points so that my data points are read  one-by-one
		//	by the child.  Otherwise, several data points would end up on the same message.
		//	In reality, you would have to deal with that, but this is a simple progamming problem,
		//	a distraction from the main point of this program.
		usleep(500000);
	}

	//	Now send an "END" message to all child processes so that they know that they
	//	have received all their data
	for (unsigned int childIndex=0; childIndex<NUM_CHILDREN; childIndex++)
	{
		if (write(parentToChild[childIndex][WRITE_END], "END", 4) == -1)
		{
			perror("Final write on parentToChild failed");
			exit(6);
		}
	}

	//	Again, the sleeping is only done in order not to mess up
	//	the stdout display
	usleep(1000000);
	
	//--------------------------------------------------------------------
	//	Wait to hear back from child processes
	//--------------------------------------------------------------------
	for (unsigned int childIndex=0; childIndex<NUM_CHILDREN; childIndex++)
	{
		char message[BUFSIZ];
		if (read(childToParent[childIndex][READ_END], message, BUFSIZ) == -1)
		{
			perror("Read CtP");
			exit(14);
		}
		//	extract information from the message
		int checkIndex;
		float result;
		sscanf(message, "%d%f", &checkIndex, &result);
		
		// checkIndex should match childIndex
		if (checkIndex != childIndex)
		{
			perror("Child index and message don't match");
			exit(15);
		}
		cout << "Parent process received value " << result <<
				" from Child " << childIndex << endl;
				
		//	In this simple problem, the child process terminates after reporting
		//	its result, so we can wait for termination now
		int statusVal;
		pid_t termProcess = waitpid(childPid[childIndex], &statusVal, 0);
		cout << "Parent process got termination signal from Child " <<
				childIndex << endl;
		(void) termProcess;
	}

	return 0;
}

//----------------------------------------------------
//	This is going to be run by Child <index>
//----------------------------------------------------
void childFunction(int index)
{
	//	close end of the pipes that I don't need
	close(parentToChild[index][WRITE_END]);
	close(childToParent[index][READ_END]);

	//	I need a List to store my data.  Here I only add to the list, never
	//	remove, so a vector seems more appropriate.
	vector<MyStruct> dataV;
	
	//	get data from the parentToChild pipe
	bool stillGoing = true;
	while (stillGoing)
	{
		//	get a message from the parent
		char message[BUFSIZ];
		if (read(parentToChild[index][READ_END], message, BUFSIZ) == -1)
		{
			perror("Read PtC failed");
			exit(4);
		}

		//-------------------------------------------------------
		//	Now I need to interpret the message I received
		//-------------------------------------------------------
		//	replace by 0 to run the stringstream version
		#if 0
			//	Version 1: using good old C-style sscanf
			char word[10];
			sscanf(message, "%s", word);
			if (strcmp(word, "DATA")==0)
			{
				//	This time I also read the data
				MyStruct obj;
				sscanf(message, "%s%f%d", word, &(obj.x), &(obj.m));
				//	and push it on my List
				dataV.push_back(obj);
			}
			else if (strcmp(word, "END")==0)
			{
				stillGoing = false;
			}
			else
			{
				perror("Invalid command");
				exit(7);
			}
		#else
			//	Version 2:  using more modern C++ stringstream
			stringstream ss(message);
			string word;
			ss >> word;
			if (word.compare("DATA")==0)
			{
				//	This time I also read the data
				MyStruct obj;
				ss >> obj.x >> obj.m;
				//	and push it on my List
				dataV.push_back(obj);
			}
			else if (word.compare("END")==0)
			{
				stillGoing = false;
			}
			else
			{
				perror("Invalid command");
				exit(7);
			}
		#endif
	}

	//-------------------------------------------------------
	//	process data.  Here I compute the sum of the x^m
	//-------------------------------------------------------
	float sum = 0.f;
	for (unsigned int k=0; k<dataV.size(); k++)
	{
		//	most math operators have a float version
		sum += powf(dataV[k].x, dataV[k].m);
	}

	//-------------------------------------------------------
	//	send result on childToParent
	//-------------------------------------------------------
	//	Here, I compose a message "<child index> result"
	ostringstream pipeSs;
	
	pipeSs << index << " " << sum;
	string messageBack = pipeSs.str();
	if (write(childToParent[index][WRITE_END], messageBack.c_str(), messageBack.length()) == -1)
	{
		perror("Write CtP failed");
		exit(9);
	}

	//----------------------------------------------------------------------
	//	Here, just for debugging/show purposes, I print out what was on the
	//	child's list, and what it computed and sent back to the parent.
	//----------------------------------------------------------------------
	//	Because the child processes are going to try writing at the same time to
	//	the standard output, I make them write into a string, and write the string
	//	in one shot to the standard output
	ostringstream stdoutSs;
	stdoutSs << "Data received by Child " <<  index << ":\n";
	for (unsigned int k=0; k<dataV.size(); k++)
	{
		stdoutSs << "\t" << dataV[k].x << "  " << dataV[k].m << endl;
	}
	stdoutSs << "Child " << index << " computed: " << sum << " sent it to the parent,";
	stdoutSs << " and terminated." << endl << flush;
	//	Now we are ready to output the string, hopefully in one shot, to stdout
	cout << stdoutSs.str();

	exit(0);
}

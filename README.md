# CSC 412 Final
This repository stores the final versions for all of my work in CSC 412: Operating Systems and Networks at URI. The earlier projects were mostly completed in C or Bash, while later ones allowed us to use C++. I will highlight a few of the more impressive/difficult/technically interesting projects below. It should also be noted that many of these projects are also documented with Doxygen, should a more in-depth explanation of any particular project be desired.


## Prog_05
This project was mostly about developing for a server-client architecture, and so the problem it solves is not very practical. If we say that a source file, or any text file really, has been scrambled, with each line divided first into a group, and then given an order , the goal of this program is to read a directory containing these scrambled text files, and determine how many subprocesses are needed. In order to invoke any of the versions, simply call launcher.sh with the path to a directory containing appropriately formatted files, and an output path, and it will then handle checking the files, and calling the main server process.

## Prog_06 
This project was my first foray into multithreading, and I'm rather proud of the results. Although I can't claim credit for the graphics code, (most of that goes to Professor Jean-Yves Herve at URI) all of the code that actually drives this program was written by me. Once the executable has been built and run, the user will be presented with a window that displays "ink levels" on the right, and a grid with a few arrows on it on the left. These arrows will quickly start to move around the grid, leaving behind a trail of pale red, green, or blue. The user will also notice that the "ink containers" all refill themselves automatically, although if that's not quick enough, pressing r, g, or b will add a bit of ink to the corresponding virtual container. This application is entirely multithreaded, with each little arrow random choosing a direction and length of travel in its own thread. Because this entire project was about teaching how to properly thread, each of the "ink containers", as well as each grid suqare, has been locked to prevent simultaneous use of the resource. This can be observed on the grid by noticing that if two arrows "walk" into each other, they will deadlock. Although this project is a little light on user interaction, it does do a great job of showing off basic multi-threaded programming.

## FinalProject
Another multithreading program, this one also deals with little "robots" travelling across a grid. Rather than idly painting the grid red, green, or blue, these robots have a job; they must push a box into a door, before themselves travelling through those same doors. This project has rather limited user interaction, with escape being able to terminate the program, and ',' and '.' slowing down and speeding up the simulation, respectively. Because this project is also about resource control and multithreading, in the final version, (the other two being incremental steps,) each robot is also managed by a separate thread, and robots and/or boxes cannot share spaces. 

For all versions, the only command you need to invoke is g++ main.cpp, with any output flags you like.
For versions 3-5, the main server process will handle compiling the two other source files, using the 
system() command to call g++. You may also feel free to avail yourself of compileAndRun.sh, which will,
like it says, compile the main, then run it using argument you provided to the script. Please do note 
that that script is a personal convenience function, and so if it is called with an improper number of
arguments, it does no error checking, and the main function will start and think it has enough arguments
until it hits an error.
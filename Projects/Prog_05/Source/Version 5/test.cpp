#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstring>

using namespace std;

void makePipes(int numProcesses){
    string directoryName = "/tmp/prog_05_pipes/";
    mkdir(directoryName.c_str(), 0700);
    for(int i = 0; i < numProcesses; i++){
        string pipeName = to_string(i);
        pipeName.insert(0, directoryName);
        pipeName.append(".pipe");
        mkfifo(pipeName.c_str(), 0666);
    }
}

void deletePipes(){
    system("rm -r /tmp/prog_05_pipes");
}

int main(){
    makePipes(5);
    deletePipes();
}
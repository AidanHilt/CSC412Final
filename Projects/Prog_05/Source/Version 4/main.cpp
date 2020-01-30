#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <cstring>

using namespace std;

//-------------------------------------
//          Function Declarations
//-------------------------------------

/**
 * @param direct The name of the directory whose content we
 * want to access
 * @return A vector of std::strings, each of which represents a 
 * text file in the given data directory
 */
vector<string> getDirectoryContents(char* direct);

/**
 * @param numItems The number of items that should be worked with in total,
 * i.e. the number of text files in the data directory.
 * @param numProcesses The total number of processes that the program should
 * call later
 * @return An integer array. It's length will equal numProcesses, and each
 * index will represent the number of files that each process is meant to handle.
 * (i.e. if index 0 is equal to 12, then process zero will handle the first
 * twelve items)
 * These are not guaranteed to be equal, and in the case of a remainder, the
 * function will start adding one to the number of items to be handled at each
 * index, starting from zero, and working up
 */
int* defineNumItems(long unsigned int numItems, long unsigned int numProcesses);

/**
 * @brief Get the contents of the file indicated by file
 * 
 * @param file A string representing the file whose contents
 * we want
 * @return vector<string> A vector representing the contents
 * of the file
 */
vector<string> getContentsOfFile(string file);

/**
 * @param files A list of files that should be parsed
 * @return A vector storing the contents of the files as
 * strings. The indices of the outputs correspond,
 * so that the string at index 0 is equal to the
 * contents of the file at index 0
 */
vector<string> getContentsOfFiles(vector<string> files);

/**
 * @param numTasks An array of the same length as numProcesses. The value at each 
 * index represents the number of files that should be handled by that "process"
 * @param numProcesses The number of processes to be launched
 * @param output The vector of string vectors that stores the files to be handled
 * by each process
 * @param input The list of files 
 */
void dispatchFilesInBatches(int* numTasks, int numProcesses, vector<vector<string>> &output,
    vector<string> input);

/**
 * @brief Writes the given string to the given file
 * 
 * @param file A string representing the file to be written to
 * @param str The string we want to write to the file
 * @return true The write succeeded
 * @return false The write failed
 */
bool writeStringToFile(string file, string str);

/**
 * @brief Compiles the dispatcher.cpp and sorter.cpp files used for exec calls
 * 
 */
void compileExecutables();

/**
 * @brief Deletes temp text files and executables used during the running of 
 * the program
 * 
 * @param numProcesses The number of processes, which will tell you how many
 * text files to delete
 */
void cleanUp(int numProcesses);

/**
 * @brief Makes pipes in a directory in /tmp/, each one
 * corresponding to a process that should be created
 * 
 * @param numProcesses The number of processes that
 * need pipes
 * 
 * @return A vector containing the name of the
 * pipe files
 */
vector<string> makePipes(int numProcesses);

/**
 * @brief Deletes the pipes in /tmp by removing
 * the directory
 */
void deletePipes();

/**
 * @brief Loops through a vector of pipe names,
 * at each waiting until the finished signal
 * is sent from the child process
 * 
 * @param pipeNames The names of all the FIFO
 * files that need to be checked
 */
void readFinishedSignals(vector<string> pipeNames);

/**
 * @brief Tells all the child processes that
 * their siblings have finished, and they can
 * dispatch to their children
 * 
 * @param pipeNames The names of all the 
 * pipes that need to be informed
 */
void signalReady(vector<string> pipeNames);

//-------------------------------------
//          Function definitions
//-------------------------------------

vector<string> getDirectoryContents(char* direct){
    DIR* directory = opendir(direct);
    if (directory == NULL){
		cout << "data folder " << direct << " not found" << endl;
		exit(1);
	}
	
    struct dirent* entry;
    vector<string>  fileNames;
    while ((entry = readdir(directory)) != NULL){
        const char* name = entry->d_name;
        if ((name[0] != '.') && (entry->d_type == DT_REG)){
			fileNames.push_back(string(direct) + string(entry->d_name));
        }
    }
	closedir(directory);
    return fileNames;
}

int* defineNumItems(long unsigned int numItems, long unsigned int numProcesses){
    int* returnVal = new int[numProcesses];
    for(unsigned int i = 0; i < numProcesses; i++){
        returnVal[i] = static_cast<int>(numItems / numProcesses);
    }
    int remainder = static_cast<int>(numItems % numProcesses);

    long unsigned int currIndex = 0;
    while(remainder > 0){
        returnVal[currIndex]++;
        currIndex++;
        currIndex = currIndex % numProcesses;
        remainder--;   
    }

    return returnVal;
}

vector<string> getContentsOfFile(string file){
    vector<string> output(0);
    char input[512];
    ifstream inputStream;

    inputStream.open(file);
    inputStream.getline(input, 512);

    while(inputStream.good()){
        output.push_back(string(input));
        inputStream.getline(input, 512);
    }

    inputStream.close();
    return output;
}

vector<string> getContentsOfFiles(vector<string> files){
    vector<string> output;
    char input[512];
    ifstream inputStream;

    for(unsigned int i = 0; i < files.size(); i++){
        inputStream.open(files[i]);
        inputStream.getline(input, 512);

        while(inputStream.good()){
            output.push_back(string(input));
            inputStream.getline(input, 512);
        }

        inputStream.close();
    }
    return output;
}

void dispatchFilesInBatches(int* numTasks, int numProcesses, vector<vector<string>> &output,
    vector<string> input){
        auto beginning = input.begin();
        for(int i = 0; i < numProcesses; i++){
            if(fork() == 0){
                vector<string> op(beginning, beginning + numTasks[i]);
                char** operations = new char *[op.size() + 2];
                operations[0] = const_cast<char*>(to_string(i).c_str());
                for(unsigned int k = 0; k < op.size(); k++){
                    operations[k + 1] = const_cast<char*>(op[k].c_str());
                }

                operations[op.size() + 1] = NULL;
                const_cast<const char**>(operations);
                execv("./dispatcher", operations);
                std::cout << "Exec failed with error: " << strerror(errno) << "\n";
                exit(1);
            }else{
                beginning += numTasks[i];
            }
        }
}

bool writeStringToFile(string file, string str){
    ofstream out;
    out.open(file, ios::trunc);
    if(out.fail()) return false;
    out << str << std::endl;
    out.close();
    return true;
}

void concatenateFiles(vector<string> pipeNames, string fileName){
    vector<int> descriptors(pipeNames.size());
    for(int i = 0; i < pipeNames.size(); i++){
        descriptors[i] = open(pipeNames[i].c_str(), O_RDONLY);
    }
    vector<string> contents;

    char buffer[2048];
    memset(buffer, 0, 2048);
    for(int i = 0; i < descriptors.size(); i++){
        read(descriptors[i], buffer, 2048);
        contents.push_back(buffer);
        memset(buffer, 0, 2048);
    }

    string out;
    for(string s : contents){
        out.append(s);
    }

    writeStringToFile(fileName, out);
    
}

void compileExecutables(){
    system("g++ -o dispatcher dispatcher.cpp");
    system("g++ -o sorter sorter.cpp");
}

void cleanUp(int numProcesses){
    for(int i = 0; i < numProcesses; i++){
        string fileName = ".txt";
        fileName.insert(0, to_string(i));
        remove(fileName.c_str());
    }

    deletePipes();

    remove("dispatcher");
    remove("sorter");
}

vector<string> makePipes(int numProcesses){
    vector<string> out;
    string directoryName = "/tmp/prog_05_pipes/";
    mkdir(directoryName.c_str(), 0700);
    for(int i = 0; i < numProcesses; i++){
        string pipeName = to_string(i);
        pipeName.insert(0, directoryName);
        pipeName.append(".pipe");
        mkfifo(pipeName.c_str(), 0666);
        out.push_back(pipeName);
    }

    return out;
}

void deletePipes(){
    system("rm -r /tmp/prog_05_pipes");
}

void readFinishedSignals(vector<string> pipeNames){
    int * descriptors = new int[pipeNames.size()];
    for(int i = 0; i < pipeNames.size(); i++){
        descriptors[i] = open(pipeNames[i].c_str(), O_RDONLY | O_NONBLOCK);
    }
    int finishedProcesses = 0;

    char buffer[512];
    while(finishedProcesses < pipeNames.size()){
        for(int i = 0; i < pipeNames.size(); i++){
            read(descriptors[i], buffer, 512);
            if(strcmp(buffer, "FIN") == 0){
                finishedProcesses ++;
                if(finishedProcesses >= pipeNames.size()) break;
            }
            memset(buffer, 0, 512);
        }
    }

    for(int i = 0; i < pipeNames.size(); i++){
        close(descriptors[i]);
    }

    delete descriptors;
}

void signalReady(vector<string> pipeNames){
    for(string s : pipeNames){
        int fd = open(s.c_str(), O_WRONLY);
        write(fd, "GO AHEAD", strlen("GO AHEAD"));
        close(fd);
    }
}


int main(int argc, char* argv[]){
    if(argc != 4){
        std::cout << "This process requires 3 aruments: numProcesses dataFolder outFileName\n";
        exit(1);
    }

    compileExecutables();

    int numProcesses = atoi(argv[1]);
    vector<string> output = getDirectoryContents(argv[2]);

    vector<string> fileContents = getContentsOfFiles(output);
    
    int *tasks = defineNumItems(output.size(), numProcesses);
    vector<vector<string>> operationsLists(numProcesses, vector<string>(0));

    vector<string> pipeNames = makePipes(numProcesses);

    dispatchFilesInBatches(tasks, numProcesses, operationsLists, fileContents);

    readFinishedSignals(pipeNames);

    signalReady(pipeNames);

    concatenateFiles(pipeNames, argv[3]);

    cleanUp(numProcesses);

    delete tasks;
}
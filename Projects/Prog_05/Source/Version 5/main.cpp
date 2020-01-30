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

/**
 * @brief The server process
 * @file
 * 
 */
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
 * @brief In this version, this function, using the prearranged number of files 
 * assigned to each process, the contents of the files, and a vector to store the
 * output, assigns files to each process, then forks out and calls dispatcher for
 * those files.
 */
void dispatchFilesInBatches(int* numTasks, int numProcesses, vector<string> input);

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
 * @brief Deletes the executables compiled
 * by this program, along with calling 
 * deletePipes() to clear the pipes
 */
void cleanUp();

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
 * the directories
 */
void deletePipes();

/**
 * @brief Continuously waits for input 
 * from each of the child processes.
 * When given something for another
 * process, it adds that to a a vector
 * of strings for that file
 * 
 * @param pipeNames The names of all the FIFO
 * files that need to be checked
 * @return A vector storing string vectors,
 * each one representing all the files
 * that need to be handled by a given process
 */
vector<vector<string>> handleClientDispatches(vector<string> pipeNames);

/**
 * @brief Distributes the lines each child, dispatcher
 * process needs back to it
 * 
 * @param pipeNames A vector with the names of
 * all the pipes, each one representing 
 * a process
 * @param fileContents The vector
 * storing the lines that must be distributed
 */
void distributeToClients(vector<string> pipeNames, vector<vector<string>> fileContents);

/**
 * @brief Creates a new vector, then goes
 * through the old one. At each, it assigns 
 * the string to the appropriate "process"
 * in the new vector, then replaces the 
 * old vector with the new one
 * 
 * @param processLinesList A vector, passed
 * by reference, storing vectors of strings,
 * each of which represent the input of one
 * process
 * 
 */
void assignLinesToProcess(vector<vector<string>> &processLinesList);

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
    char input[2048];
    ifstream inputStream;

    inputStream.open(file);
    inputStream.getline(input, 2048);

    while(inputStream.good()){
        output.push_back(string(input));
        inputStream.getline(input, 2048);
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

        output.push_back(input);

        while(inputStream.good()){
            inputStream.getline(input, 512);
            memset(input, 0, 512);
        }

        inputStream.close();
    }
    return output;
}

void dispatchFilesInBatches(int* numTasks, int numProcesses, vector<string> input){
    auto beginning = input.begin();
    for(int i = 0; i < numProcesses; i++){
        if(fork() == 0){
            execl("./dispatcher", const_cast<char*>(to_string(i).c_str()), NULL);
            std::cout << "Exec failed with error: " << strerror(errno) << "\n";
            exit(1);
        }else{
            string directory = string("/tmp/prog_05_pipes/").append(to_string(i)).append(".pipe");

            auto end = beginning + numTasks[i];
            if(end > input.end()) end = input.end();
            vector<string> contents(beginning, end);

            char buffer[9];
            for(string s : contents){
                int fd = open(directory.c_str(), O_WRONLY);
                write(fd, s.c_str(), s.size());
                close(fd);

                fd = open(directory.c_str(), O_RDONLY);
                read(fd, buffer, 9);
                if(strcmp(buffer, "RECEIVED") != 0){
                    cout << "Error communicating from child to grandchild. Aborting.\n";
                    exit(1);
                }
                memset(buffer, 0, 9);
                close(fd);
            }

            int fd = open(directory.c_str(), O_WRONLY);
            write(fd, "FIN", 4);
            close(fd);
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

    char buffer[65536];
    memset(buffer, 0, 65536);
    for(int i = 0; i < descriptors.size(); i++){
        read(descriptors[i], buffer, 65536);
        contents.push_back(buffer);
        memset(buffer, 0, 65536);
    }

    string out;
    for(string s : contents){
        out.append(s);
    }

    for(int i = 0; i < pipeNames.size(); i++){
        close(descriptors[i]);
    }


    writeStringToFile(fileName, out);
    
}

void compileExecutables(){
    system("g++ -o dispatcher dispatcher.cpp");
    system("g++ -o sorter sorter.cpp");
}

void cleanUp(){
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
    system("rm -r /tmp/prog_05_child_pipes");
}

vector<vector<string>> handleClientDispatches(vector<string> pipeNames){
    vector<vector<string>> out(pipeNames.size(), vector<string>());
    int finishedProcesses = 0;

    char buffer[2048];
    memset(buffer, 0, 2048);

    vector<int> loop;

    for(int i = 0; i < pipeNames.size(); i++){
        loop.push_back(i);
    }

    while(finishedProcesses < pipeNames.size()){
        for(int i : loop){
            int fd = open(pipeNames[i].c_str(), O_RDONLY);
            read(fd, buffer, 2048);

            if(strcmp(buffer, "FIN") == 0){
                finishedProcesses ++;
                loop.erase(find(loop.begin(), loop.end() + 1, i));
                close(fd);
                break;
            }else{
                out[i].push_back(buffer);
                memset(buffer, 0, 2048);
            }

            close(fd);

            if(strcmp(buffer, "FIN") != 0){
                fd = open(pipeNames[i].c_str(), O_WRONLY);
                write(fd, "RECEIVED", strlen("RECEIVED") + 1);
                close(fd);
            }else{
                memset(buffer, 0, 2048);
            }
        }
    }

    return out;
}

void distributeToClients(vector<string> pipeNames, vector<vector<string>> fileContents){
    char buffer[9];
    for(int i = 0; i < pipeNames.size(); i++){
        for(string os : fileContents[i]){
            int fd = open(pipeNames[i].c_str(), O_WRONLY);
            write(fd, os.c_str(), os.size());
            close(fd);
            
            fd = open(pipeNames[i].c_str(), O_RDONLY);
            read(fd, buffer, 9);
            if(strncmp(buffer, "RECEIVED", 9) != 0){
                cout << "Something went wrong with the client communicating back to the server\nAborting...\n";
                exit(1);
            }
            close(fd);
            memset(buffer, 0, 9);
        }
        int fd = open(pipeNames[i].c_str(), O_WRONLY);
        write(fd, "GO AHEAD", strlen("GO AHEAD"));
        close(fd);
    }
}

void assignLinesToProcess(vector<vector<string>> &processLinesList){
    vector<vector<string>> finishedList(processLinesList.size(), vector<string>());
    for(vector<string> v : processLinesList){
        for(string s : v){
            unsigned int index = static_cast<unsigned int>(stoi(s.substr(0, s.find_first_not_of("0123456789"))));
            string dispatchString = s.substr(s.find_first_not_of("0123456789") + 1, string::npos);
            finishedList[index].push_back(dispatchString);
        }
    }

    processLinesList = finishedList;
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

    vector<string> pipeNames = makePipes(numProcesses);

    dispatchFilesInBatches(tasks, numProcesses, fileContents);

    vector<vector<string>> contents = handleClientDispatches(pipeNames);

    assignLinesToProcess(contents);

    distributeToClients(pipeNames, contents);

    concatenateFiles(pipeNames, argv[3]);

    cleanUp();

    delete tasks;
}
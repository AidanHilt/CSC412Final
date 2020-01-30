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
 * @param input A vector of vectors of strings,
 * each of which should be a line from the input
 * file
 * Given the input, sorts each vector using std::sort
 * and the compareLines() function
 */
void sortAllLists(unsigned int numFiles);

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

void sortAllLists(unsigned int numFiles){
    for(unsigned int i = 0; i < numFiles; i++){
        int status = fork();
        if(status == 0){
            const char* arg = to_string(i).c_str();
            execl("./sorter", arg);
            std::cout << "Exec failed with error: \n" << strerror(errno) << "\n";
            exit(1);
        }else{
            wait(NULL);
        }
    }
}

void dispatchFilesInBatches(int* numTasks, int numProcesses, vector<vector<string>> &output,
    vector<string> input){
        auto beginning = input.begin();
        for(int i = 0; i < numProcesses; i++){
            if(fork() == 0){
                vector<string> op(beginning, beginning + numTasks[i]);
                char** operations = new char *[op.size() + 1];
                for(unsigned int k = 0; k < op.size(); k++){
                    operations[k] = const_cast<char*>(op[k].c_str());
                }

                operations[op.size()] = NULL;
                const_cast<const char**>(operations);
                execv("./dispatcher", operations);
                std::cout << "Exec failed with error: \n" << strerror(errno) << "\n";
                exit(1);
            }else{
                wait(NULL);
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

void concatenateFiles(int numProcesses, string fileName){
    string out;
    for(int i = 0; i < numProcesses; i++){
        string fileName = to_string(i) + ".txt";
        ifstream inFile(fileName);
        char c;

        while(inFile.get(c)){
            out += (c);
        }
    }

    writeStringToFile(fileName, out);
}

void compileExecutables(){
    system("g++ -o dispatcher dispatcher.cpp");
    system("g++ -o sorter sorter.cpp");
}

void cleanUp(int numProcesses){
    for(int i = 0; i < numProcesses; i++){
        string fileName = to_string(i) + ".txt";
        remove(fileName.c_str());
    }

    remove("dispatcher");
    remove("sorter");
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

    dispatchFilesInBatches(tasks, numProcesses, operationsLists, fileContents);

    sortAllLists(numProcesses);

    concatenateFiles(numProcesses, argv[3]);

    cleanUp(numProcesses);

    delete tasks;
}
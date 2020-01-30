#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

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
 * @param str1 The first string to be compared
 * @param str2 The second string to be compared
 * @return True if the first item goes before the 
 * second one in a list, and false otherwise
 * Given these two input strings, it will attempt to extract a 
 * number by starting right after the first space, and looking 
 * until a character is not a number. It does this for both strings,
 * then attempting to convert these into ints. After that, it compares
 * the two ints, and returns a value based on that comparison
 */
bool compareLines(const string &str1, const string &str2);

/**
 * @param input The string to be cleaned.
 * "Cleans" the given string by appending a newline
 * character, and then clearing out the first two numbers
 * and the accompanying spaces. This function does 0
 * error checking, so calling it on a string not
 * of the format given in the assignment is 
 * undefined behavior, which includes a crash
 */
void cleanLine(string &input);

/**
 * @param input A vector of vectors of strings,
 * each of which should be a line from the input
 * file
 * Given the input, loops through every string 
 * in every vector, calling the cleanLine function
 * on each
 */
string cleanLines(vector<string> &input);

/**
 * @param input A vector of vectors of strings,
 * each of which should be a line from the input
 * file
 * Given the input, sorts each vector using std::sort
 * and the compareLines() function
 */
void sortAllLists(unsigned int numFiles);

/**
 * @param input A list of the contents of files generated
 * by getContentsOfFiles(), which need to be assigned to their
 * relevant process
 * Given the input, appends each string to its appropriate 
 * spot in the appropriate output file, representing the 
 * "process" that should operate on it
 */
void dispatchTextFiles(vector<string> input);

/**
 * @param numTasks An array of the same length as numProcesses. The value at each 
 * index represents the number of files that should be handled by that "process"
 * @param numProcesses The number of processes to be launched
 * @param output The vector of string vectors that stores the files to be handled
 * by each process
 * @param input The list of files 
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

void dispatchTextFiles(vector<string> input){
    for(unsigned int i = 0; i < input.size(); i++){
        unsigned int index = static_cast<unsigned int>(stoi(input[i].substr(0, input[i].find_first_not_of("0123456789"))));
        ofstream out;
        out.open(to_string(index).append(".txt"), ios_base::app);
        out << input[i] << std::endl;
        out.close();
    }
}

void dispatchFilesInBatches(int* numTasks, int numProcesses, vector<string> input){
        auto beginning = input.begin();
        for(int i = 0; i < numProcesses; i++){
            if(fork() == 0){
                vector<string> op(beginning, beginning + numTasks[i]);
                dispatchTextFiles(op);
                exit(0);
            }else{
                wait(NULL);
                beginning += numTasks[i];
            }
        }
}

bool compareLines(const string &str1, const string &str2){
    int int1 = stoi(str1.substr(str1.find_first_of(" "),  str1.find_first_not_of("0123456789", str1.find_first_of(" ") + 1)));
    int int2 = stoi(str2.substr(str2.find_first_of(" "),  str2.find_first_not_of("0123456789", str2.find_first_of(" ") + 1)));

    bool outVal = (int1 < int2);

    return outVal;
}

void cleanLine(string &input){
    input.append("\n");
    input = input.substr(input.find_first_of(" \n", input.find_first_of(" ") + 1));
}

string cleanLines(vector<string> &input){    
    string outString = "";

    for(unsigned int i = 0; i < input.size(); i++){
        cleanLine(input[i]);
        outString.append(input[i]);
    }

    return outString;
}

void sortAllLists(unsigned int numFiles){
    for(unsigned int i = 0; i < numFiles; i++){
        int status = fork();
        if(status == 0){
            string fileName = to_string(i) + ".txt";
            vector<string> contents = getContentsOfFile(fileName);
            sort(contents.begin(), contents.end(), compareLines);
            
            string contentStr = cleanLines(contents);
            writeStringToFile(fileName, contentStr);
            exit(0);
        }else{
            wait(NULL);
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

void cleanUp(int numProcesses){
    for(int i = 0; i < numProcesses; i++){
        string fileName = to_string(i) + ".txt";
        remove(fileName.c_str());
    }
}

int main(int argc, char* argv[]){
    if(argc != 4){
        std::cout << "This process requires 3 aruments: numProcesses dataFolder outFileName\n";
        exit(1);
    }
    int numProcesses = atoi(argv[1]);
    vector<string> output = getDirectoryContents(argv[2]);

    vector<string> fileContents = getContentsOfFiles(output);
    
    int *tasks = defineNumItems(output.size(), numProcesses);

    dispatchFilesInBatches(tasks, numProcesses, fileContents);

    sortAllLists(numProcesses);

    concatenateFiles(numProcesses, argv[3]);

    cleanUp(numProcesses);

    delete tasks;
}
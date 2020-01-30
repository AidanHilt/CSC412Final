#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <cstring>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

/**
 * @brief The sorter process
 * @file
 * 
 */
//-------------------------------------
//          Function Declarations
//-------------------------------------

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
 * @brief Get the contents of the file indicated by file
 * 
 * @param file A string representing the file whose contents
 * we want
 * @return vector<string> A vector representing the contents
 * of the file
 */
vector<string> receiveInputFromParent(string file);

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
 * @return A string made up of the contents of
 * input, concatenated together with 
 * Given the input, loops through every string 
 * in every vector, calling the cleanLine function
 * on each
 */
void cleanLines(vector<string> &input);

/**
 * @brief Returns the sorted content
 * back to the parent, one at a time,
 * waiting for recieved confirmation
 * after each write
 * 
 * @param contents A vector of strings,
 * each of which needs to be returned 
 * to the parent
 * @param pipe The pipe we want to 
 * write the content to.
 */
void returnSortedContentToParent(vector<string> contents, string pipe);

//-------------------------------------
//          Function definitions
//-------------------------------------

bool compareLines(const string &str1, const string &str2){
    int int1 = stoi(str1.substr(str1.find_first_of(" "),  str1.find_first_not_of("0123456789", str1.find_first_of(" ") + 1)));
    int int2 = stoi(str2.substr(str2.find_first_of(" "),  str2.find_first_not_of("0123456789", str2.find_first_of(" ") + 1)));

    bool outVal = (int1 < int2);

    return outVal;
}

 vector<string> receiveInputFromParent(string pipeName){
    vector<string> output(0);
    char input[2048];

    memset(input, 0, 2048);
    int fd = open(pipeName.c_str(), O_RDONLY);
    read(fd, input, 2048);
    while(strcmp(input, "FIN") != 0){
        output.push_back(input);
        close(fd);

        fd = open(pipeName.c_str(), O_WRONLY);
        write(fd, "RECEIVED", 9);
        close(fd);

        memset(input, 0, 2048);
        fd = open(pipeName.c_str(), O_RDONLY);
        read(fd, input, 2048);
    }

    close(fd);
    return output;
}

void cleanLine(string &input){
    input.append("\n");
    input = input.substr(input.find_first_of(" \n", input.find_first_of(" ") + 1));
}

void cleanLines(vector<string> &input){

    for(unsigned int i = 0; i < input.size(); i++){
        cleanLine(input[i]);
    }
}

void returnSortedContentToParent(vector<string> contents, string pipe){
    char buffer[9];
        for(string s : contents){
            int fd = open(pipe.c_str(), O_WRONLY);
            write(fd, s.c_str(), s.size());
            close(fd);

            fd = open(pipe.c_str(), O_RDONLY);
            read(fd, buffer, 9);
            if(strcmp(buffer, "RECEIVED") != 0){
                cout << "Error communicating from grandchild to child. Aborting.\n";
                exit(1);
            }
            memset(buffer, 0, 9);
            close(fd);
        }

        int fd = open(pipe.c_str(), O_WRONLY);
        write(fd, "FIN", 4);
        close(fd);
}

int main(int argc, char* argv[]){
    string pipeName(argv[0]);
    pipeName.append(".pipe");
    pipeName.insert(0, "/tmp/prog_05_child_pipes/");
    vector<string> contents = receiveInputFromParent(pipeName);
    
    sort(contents.begin(), contents.end(), compareLines);
    
    cleanLines(contents);

    returnSortedContentToParent(contents, pipeName);

    exit(0);
}
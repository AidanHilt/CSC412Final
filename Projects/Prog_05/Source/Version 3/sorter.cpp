#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

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
vector<string> getContentsOfFile(string file);

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
string cleanLines(vector<string> &input);

/**
 * @brief Writes the given string to the given file
 * 
 * @param file A string representing the file to be written to
 * @param str The string we want to write to the file
 * @return true The write succeeded
 * @return false The write failed
 */
bool writeStringToFile(string file, string str);

//-------------------------------------
//          Function definitions
//-------------------------------------

bool compareLines(const string &str1, const string &str2){
    int int1 = stoi(str1.substr(str1.find_first_of(" "),  str1.find_first_not_of("0123456789", str1.find_first_of(" ") + 1)));
    int int2 = stoi(str2.substr(str2.find_first_of(" "),  str2.find_first_not_of("0123456789", str2.find_first_of(" ") + 1)));

    bool outVal = (int1 < int2);

    return outVal;
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

bool writeStringToFile(string file, string str){
    ofstream out;
    out.open(file, ios::trunc);
    if(out.fail()) return false;
    out << str << std::endl;
    out.close();
    return true;
}

int main(int argc, char* argv[]){
    string fileName(argv[0]);
    fileName.append(".txt");
    vector<string> contents = getContentsOfFile(fileName);
    sort(contents.begin(), contents.end(), compareLines);
    
    string contentStr = cleanLines(contents);
    writeStringToFile(fileName, contentStr);
    exit(0);

}
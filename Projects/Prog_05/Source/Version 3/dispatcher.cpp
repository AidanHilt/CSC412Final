#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

/**
 * @param input A list of the contents of files generated
 * by getContentsOfFiles(), which need to be assigned to their
 * relevant process
 * Given the input, appends each string to its appropriate 
 * spot in the appropriate output file, representing the 
 * "process" that should operate on it
 */
void dispatchTextFiles(vector<string> input);

void dispatchTextFiles(vector<string> input){
    for(unsigned int i = 0; i < input.size(); i++){
        unsigned int index = static_cast<unsigned int>(stoi(input[i].substr(0, input[i].find_first_not_of("0123456789"))));
        ofstream out;
        out.open(to_string(index).append(".txt"), ios_base::app);
        out << input[i] << std::endl;
        out.close();
    }
}

int main(int argc, char* argv[]){
    vector<string> contents;
    for(int i = 0; i < argc; i++){
        contents.push_back(argv[i]);
    }
    dispatchTextFiles(contents);
}
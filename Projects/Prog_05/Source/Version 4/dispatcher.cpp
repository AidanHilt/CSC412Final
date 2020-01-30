#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/wait.h>

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

void checkToGo(string outPipeName, const char* index){
    int fd = open(outPipeName.c_str(), O_RDONLY);
    char buffer[512];
    read(fd, buffer, 512);
    if(strncmp(buffer, "GO AHEAD", strlen("GO AHEAD")) == 0){
        int f = fork();
        close(fd);
        if(f == 0){
            execl("./sorter", index, NULL);
            std::cout << "Exec failed with error: " << strerror(errno) << "\n";
            exit(1);
        }else{
            wait(NULL);
        }
    }else{
        std::cout << "Something went wrong in communication with the server. The message provided was: " 
        << buffer << "\n" << "Terminating...\n";
        exit(1);
    }
}

void sendSortedToServer(string outPipeName, const char* index){
    string file(index);
    file.append(".txt");

    int fd = open(outPipeName.c_str(), O_WRONLY);
    ifstream inFile(file);

    string out = "";
    char c;

    while(inFile.get(c)){
        out += c;
    }

    write(fd, out.c_str(), out.size());
    close(fd);
}

int main(int argc, char* argv[]){
    vector<string> contents;
    string outPipeName = "/tmp/prog_05_pipes/";
    outPipeName.append(argv[0]);
    outPipeName.append(".pipe");

    for(int i = 1; i < argc; i++){
        contents.push_back(argv[i]);
    }

    dispatchTextFiles(contents);

    int fd = open(outPipeName.c_str(), O_WRONLY);
    write(fd, "FIN", strlen("FIN") + 1);
    close(fd);

    checkToGo(outPipeName, argv[0]);

    sendSortedToServer(outPipeName, argv[0]);

    exit(0);
}
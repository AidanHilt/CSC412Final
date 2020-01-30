#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/wait.h>
#include <sys/stat.h>

using namespace std;

/**
 * @brief The dispatcher process
 * @file
 * 
 */
//===========================
//  Function declarations
//===========================

/**
 * @brief Sends the contents of the files back up to the 
 * server for sorting to the appropriate file
 * 
 * @param input All the file contents that need to 
 * be dispatched and sorted
 * @param outPipeName The name of the pipe that
 * needs to be used to communicate with the 
 * server process
 */
void dispatchTextFiles(vector<string> input, string outPipeName);

/**
 * @brief Concatenates the sorted values returned
 * from the sorter process into a single string,
 * then writes it back up to the parent
 * 
 * @param outPipeName The name of the pipe
 * to write to
 * @param sortedContent A vector representing
 * the sorted content from the child process
 */
void sendSortedToServer(string outPipeName, vector<string> sortedContent);

/**
 * @brief Gets the content assigned to this
 * process back from the server
 * 
 * @param outPipeName The name of the pipe
 * to read from 
 * @return vector<string> A vector
 * storing all of the lines this process
 * needs to handle
 */
vector<string> getOwnContentFromServer(string outPipeName);

/**
 * @brief Forks, then execs() the 
 * child sorter process, piping
 * the contents it needs to work
 * on
 * 
 * @param contents The file contents
 * the sorter child will need
 * @param index The index of this process,
 * which the child process will need as an
 * argument
 */
void spawnSorterProcess(vector<string> contents, const char* index);

/**
 * @brief Gets the sorted, cleaned
 * input back from the child
 * 
 * @param pipeName The name of the
 * pipe to read from
 * @return vector<string> A vector
 * containing all the sorted, cleaned
 * lines
 */
vector<string> receiveInputFromChild(string pipeName);

//===========================
//  Function definitions
//===========================

void dispatchTextFiles(vector<string> input, string outPipeName){
    for(unsigned int i = 0; i < input.size(); i++){
        int fd = open(outPipeName.c_str(), O_WRONLY);
        string index = input[i].substr(0, input[i].find_first_not_of("0123456789")).append(" ");
        write(fd, index.append(input[i]).c_str(), input[i].size() + index.size());

        close(fd);

        fd = open(outPipeName.c_str(), O_RDONLY);
        char buff[2048];

        memset(buff, 0, 2048);

        read(fd, buff, 2048);

        if(strcmp(buff, "RECEIVED") != 0){
            cout << buff << "\n";
            cout << "Error on reception message from server.\nAborting...\n";
            exit(1);
        }

        close(fd);
    }
    
    int fd = open(outPipeName.c_str(), O_WRONLY);
    write(fd, "FIN", strlen("FIN") + 1);
    close(fd);
}

void sendSortedToServer(string outPipeName, vector<string> sortedContent){
    string out = "";

    for(string s : sortedContent){
        out.append(s);
    }

    int fd = open(outPipeName.c_str(), O_WRONLY);
    write(fd, out.c_str(), out.size());
    close(fd);
}

vector<string> getOwnContentFromServer(string outPipeName){
    int fd = open(outPipeName.c_str(), O_RDONLY);
    vector<string> lines;
    char buffer[2048];
    read(fd, buffer, 2048);

    while(strncmp(buffer, "GO AHEAD", strlen("GO AHEAD")) != 0){
        string appendString = buffer;
        if(appendString.find("VED") != string::npos) appendString.erase(appendString.find("VED"));
        if(appendString.find("ED") != string::npos) appendString.erase(appendString.find("ED"));
        lines.push_back(appendString);
        memset(buffer, 0, 2048);
        close(fd);
        fd = open(outPipeName.c_str(), O_WRONLY);
        write(fd, "RECEIVED", strlen("RECEIVED"));
        close(fd);
        fd = open(outPipeName.c_str(), O_RDONLY);
        read(fd, buffer, 2048);
    }

    close(fd);

    return lines;
}

void spawnSorterProcess(vector<string> contents, const char* index){
    string directory = "/tmp/prog_05_child_pipes/";
    mkdir(directory.c_str(), 0700);
    directory.append(index);
    directory.append(".pipe");
    mkfifo(directory.c_str(), 0666);

    int response = fork();

    if(response == 0){
        execl("./sorter", index, NULL);
        std::cout << "Exec failed with error: \n" << strerror(errno) << "\n";
        exit(1);
    }else{
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
    }
}

vector<string> receiveInputFromChild(string pipeName){
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

int main(int argc, char* argv[]){
    string outPipeName = "/tmp/prog_05_pipes/";
    outPipeName.append(argv[0]);
    outPipeName.append(".pipe");

    vector<string> contents = receiveInputFromParent(outPipeName);

    dispatchTextFiles(contents, outPipeName);

    vector<string> personalContents = getOwnContentFromServer(outPipeName);

    spawnSorterProcess(personalContents, argv[0]);

    vector<string> sortedContents = receiveInputFromChild(string("/tmp/prog_05_child_pipes/").append(argv[0]).append(".pipe"));

    sendSortedToServer(outPipeName, sortedContents);

    exit(0);
}
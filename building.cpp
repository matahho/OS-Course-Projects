
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include "utils.h"

#define ELECFILE "Electricity"
#define GASFILE "Gas"
#define WATERFILE "Water" 
#define NUMBER_OF_RESOURCE 3


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"


using namespace std;


int findFifoIndex (vector<string>fifos , string name){
    for (int i=0 ; i < fifos.size() ; i++)
        if (name == fifos[i]) 
            return i;
}


void sendDataToOffice(string message , string AllBuildingPath , string BuildingName){
    vector<string>fifos;
    getFifosNames (getAllDirectories(AllBuildingPath) , fifos);
    //makingFifos( getAllDirectories(AllBuildingPath) , fifos);
    
    int fd = open(("/tmp/"+BuildingName).c_str(), O_WRONLY);
    if (fd != -1) {
        write(fd, message.c_str(), message.size());
        close(fd);
        cout << GREEN <<"SEND From Building To Office"<< RESET << endl;
    } else {
        perror("Failed to open FIFO for writing");
    }

    
}





std::string receiveResponseFromOffice(const std::string& buildingName) {
    std::string pipePath = "/tmp/" + buildingName;
    char buffer[1024];

    int fd1 = open(pipePath.c_str(), O_RDONLY);
    if (fd1 == -1) {
        perror("Error opening pipe");
        exit(EXIT_FAILURE);
    }

    ssize_t bytesRead = read(fd1, buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        perror("Error reading from pipe");
        close(fd1);
        exit(EXIT_FAILURE);
    } else if (bytesRead == 0) {
        std::cerr << "Pipe closed" << std::endl;
        close(fd1);
        exit(EXIT_SUCCESS);
    }

    buffer[bytesRead] = '\0';

    close(fd1); // Close the file descriptor after reading

    return std::string(buffer);
}



int main(int argc , char* argv[]) {
    int pipe_fd[NUMBER_OF_RESOURCE][2];

    vector<string> resourceNames =  {ELECFILE, GASFILE, WATERFILE};
    vector<string> reducedText ;
    

    for (int i=0 ; i < NUMBER_OF_RESOURCE ; i++){

        // Create a pipe
        if (pipe(pipe_fd[i]) == -1) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();


        if (pid == -1) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Child process

            // Close the write end of the pipe
            close(pipe_fd[i][0]);

            // Redirect standard input to read from the pipe
            dup2(pipe_fd[i][1], STDOUT_FILENO);

            string filePath = string(argv[1]) + "/" +string(argv[2])+ "/" + resourceNames[i] + ".csv";

            char * command [] = {"./bills.out" , const_cast<char*>(filePath.c_str()) , NULL};

            
            execvp(command[0] , command);

            // If execl fails
            perror("Exec failed");
            exit(EXIT_FAILURE);
        }

        else {

            close(pipe_fd[i][1]);
            char val[1024];
            ssize_t bytesRead;

            while ((bytesRead = read(pipe_fd[i][0], val, sizeof(val) - 1)) > 0) {
                val[bytesRead] = '\0';  // Null-terminate the string
                reducedText.push_back(string(val));
            
                
            }
            
            
            close(pipe_fd[i][0]);

        }


    }
    string all;
    for (int i=0 ; i<resourceNames.size() ; i++){
        all = all+ resourceNames[i]+"\n"+reducedText[i];
    }
    printf("%s" , all.c_str());
    //sendDataToOffice(all , argv[1] , argv[2]);

    //cout << receiveResponseFromOffice(string(argv[2]));
    
    




    

    return 0;
}

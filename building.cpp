
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#define ELECFILE "Electricity"
#define GASFILE "Gas"
#define WATERFILE "Water" 
#define NUMBER_OF_RESOURCE 3


// struct ReduceText
// {
//     string name;
//     int year;
//     int month;
//     int peakHour;
//     int usageInPeadHour;
//     int totalUsage;

// };


// ReduceText reduceWorker (string text){
    
// }



using namespace std;


int main(int argc , char* argv[]) {
    int pipe_fd[3][2];

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

            string filePath = string(argv[1]) + "/" + resourceNames[i] + ".csv";
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






    

    return 0;
}

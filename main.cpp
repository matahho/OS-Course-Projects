#include <iostream>
#include <vector>
#include "csvReader.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INUPUT_SIZE 512 

using namespace std;


void listBuildings (string allBuildingPath){
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { 
        execl("/bin/ls", "ls", "-1", allBuildingPath.c_str(), (char *)0);
        perror("execl");
        exit(EXIT_FAILURE);
    } else 
        waitpid(pid, nullptr, 0);
        
}



string getRequestInfo (){
    string com , name ;
    cout << "Enter Comodity(gas/water/elec):\n";
    cin >> com;
    cout << "Enter Building name:\n";
    cin >> name;
    return "/"+ name + "/" + com; 
}




int main (int argc , char* argv[]){
    
    string allBuildingPath = argv[1];
    string input;
    while (1){
        cin >> input ; 
        if (input == "list")
            listBuildings(allBuildingPath);
        if (input == "report"){
            input = getRequestInfo();
            int pipe_fd[2];

            // Create a pipe
            if (pipe(pipe_fd) == -1) {
                perror("Pipe creation failed");
                exit(EXIT_FAILURE);
            }

            pid_t pid = fork();

            if (pid == -1) {
                perror("Fork failed");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) {

                // Close the write end of the pipe
                close(pipe_fd[0]);

                // Redirect standard input to read from the pipe
                dup2(pipe_fd[1], STDOUT_FILENO);

                string s = allBuildingPath+input+".csv";
                char * command [] = {const_cast<char*>("./building.out"),const_cast<char*>(s.c_str()),  const_cast<char*>("1") , NULL};
                execvp(command[0] , command);

                // If execl fails
                perror("Exec failed");
                exit(EXIT_FAILURE);
            } else {

                printf("Date\t\tHighHour\t\tUsageInHH\t\tTotalUsage\n");

                close(pipe_fd[1]);
                char val [1024];
                read(pipe_fd[0] , &val , sizeof(val));
                
                
                printf("%s" , val);


                close(pipe_fd[0]);
            }


        }

        else if (input == "quit")
            break;

        

    }
    

}
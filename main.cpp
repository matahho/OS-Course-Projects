#include <iostream>
#include <vector>
#include "csvReader.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>
#include "utils.h"

#define INUPUT_SIZE 512
#define ELECFILE "Electricity"
#define GASFILE "Gas"
#define WATERFILE "Water"  


#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define WHITE   "\033[37m"

using namespace std;


// void listBuildings (string allBuildingPath){
//     pid_t pid = fork();
//     if (pid == -1) {
//         perror("fork");
//         exit(EXIT_FAILURE);
//     } else if (pid == 0) { 
//         execl("/bin/ls", "ls", "-1", allBuildingPath.c_str(), (char *)0);
//         perror("execl");
//         exit(EXIT_FAILURE);
//     } else 
//         waitpid(pid, nullptr, 0);
        
// }





void listBuildings (string allBuildingPath){
    cout << GREEN << "List of all avaliable buildings :" << RESET << endl;
    vector<string> allBuildings = getAllDirectories(allBuildingPath);
    for (auto b :allBuildings)  
        cout <<BLUE<< b <<RESET<< endl ;
        
}

vector<string> getRequestInfo (){
    vector<string> com , names;

    string line;
    
    cout << "Enter Commodities (Gas/Water/Electricity/exit):" << endl;   
    while (cin >> line){
        if (line == "ex")
            break;
        else 
            com.push_back(line);
    }
    cout << "Enter Building Name (exit):" << endl;
    while (cin >> line){
        if (line == "ex")
            break;
        else 
            names.push_back(line);
    }

    vector<string> all;
    for (auto n : names)
        for (auto c : com)
            all.push_back("/"+n+"/"+c);

    return all;
}

pid_t createOfficeProcess(const std::string& allBuildingPath , string &reducedTest) {
    int pipe_fd[2];

    // Create a pipe for communication between main and office process
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
        close(pipe_fd[1]);


        dup2(pipe_fd[0], STDOUT_FILENO);

        
        execl("./office.out", "./office.out", allBuildingPath.c_str(), nullptr);


        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else {


            close(pipe_fd[1]);
            char val[2048];
            ssize_t bytesRead;

            while ((bytesRead = read(pipe_fd[0], val, sizeof(val) - 1)) > 0) {
                val[bytesRead] = '\0'; 
                reducedTest += string(val);
            }
            
        close(pipe_fd[0]);
    }

    return pid;
}



int main (int argc , char* argv[]){
    
    string allBuildingPath = argv[1];
    string input;
    vector<string> fifos;
    vector<string> allBuildingNames = getAllDirectories(allBuildingPath);
    makingFifos(allBuildingNames , fifos);
    listBuildings(allBuildingPath);
    vector<string> reducedText ;

    
    vector<string>reqInfo = getRequestInfo();
    
    
    
    
    //pid_t officePid = createOfficeProcess(allBuildingPath , reducedText );
    
    
    
    
    
    int pipe_fd[allBuildingNames.size()][2];
    for (int i = 0; i < allBuildingNames.size(); i++) {
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

            // Close the write end of the pipe in the child
            close(pipe_fd[i][0]);

            // Redirect standard input to read from the pipe
            dup2(pipe_fd[i][1], STDOUT_FILENO);

            

            char* command[] = {"./building.out", const_cast<char*>(allBuildingPath.c_str()), const_cast<char*>(allBuildingNames[i].c_str()), NULL};

            execvp(command[0], command);

            // If execl fails
            perror("Exec failed");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            close(pipe_fd[i][1]);
            char val[1024];
            ssize_t bytesRead;

            while ((bytesRead = read(pipe_fd[i][0], val, sizeof(val) - 1)) > 0) {
                val[bytesRead] = '\0';  // Null-terminate the string
                reducedText.push_back(string(val));
            
                
            }
            // Close the read end of the pipe in the parent
            close(pipe_fd[i][0]);
            waitpid(pid, NULL, 0);
        }
    }   

    cout << "REduces";
    for (auto i :reducedText)
        cout << i << endl;
}
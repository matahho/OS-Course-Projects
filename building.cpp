
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

int main(int argc , char* argv[]) {
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
        // Child process

        // Close the write end of the pipe
        close(pipe_fd[0]);

        // Redirect standard input to read from the pipe
        dup2(pipe_fd[1], STDOUT_FILENO);

        
        char * command [] = {"./bills.out" , argv[1] , argv[2] , NULL};
        execvp(command[0] , command);

        // If execl fails
        perror("Exec failed");
        exit(EXIT_FAILURE);
    } else {

        close(pipe_fd[1]);
		char val [1024];
        read(pipe_fd[0] , &val , sizeof(val));

		printf("%s" , val);

        close(pipe_fd[0]);
    }

    return 0;
}

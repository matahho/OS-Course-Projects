#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdbool.h>
#include <errno.h>


#define TCP_TOP_RANGE 9090
#define TCP_DOWN_RANGE 5050
#define WHATISYOURNAME "$whatIsYourName$"
#define IF_YOU_ARE_RESTAURANT_SAY_YOUR_NAME "$IF_RESTAURANT_NAME$"
#define SUPPLIERJOB 'S'
#define REQUEST "$request$"




#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define PURPLE  "\x1b[35m"
#define RESET_COLOR   "\x1b[0m"


typedef struct supplierStates { 
    char job ;
    bool isBusy ;
    char name [128];
    int sock ;

}Supplier;



Supplier* setupServer(int port) {
    Supplier *supp = (Supplier*)malloc(sizeof(Supplier));
    supp->isBusy = 0 ;
    supp->job = SUPPLIERJOB;
    struct sockaddr_in address;
    
    supp->sock = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(supp->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(port);

    bind(supp->sock, (struct sockaddr *)&address, sizeof(address));
    
    listen(supp->sock, 1024);

    return supp;
}

int acceptClient(int server_fd) {
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);
    client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);

    return client_fd;
}


int* AllAvailablesTCP() {
    int socket_desc;
    struct sockaddr_in server;

    // Create the socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        perror("Could not create socket");
        return NULL;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    int maxPorts = TCP_TOP_RANGE - TCP_DOWN_RANGE;
    int *OpenTCPs = (int *)malloc(maxPorts * sizeof(int));
    int count = 0;

    if (OpenTCPs == NULL) {
        perror("Memory allocation failed");
        return NULL;
    }

    for (int port = TCP_DOWN_RANGE; port <= TCP_TOP_RANGE; ++port) {
        server.sin_port = htons(port);

        if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) >= 0) {
            OpenTCPs[count++] = port;
        } else if (errno != ECONNREFUSED && errno != ETIMEDOUT) {
            perror("Error connecting to port");
        }

        close(socket_desc);
        socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_desc == -1) {
            perror("Could not recreate socket");
            break;
        }
    }

    close(socket_desc);
    return OpenTCPs;
}

int firstAllowableTCPPort(){
    int *openTCPs = AllAvailablesTCP();
    for (int i = 0; i < TCP_TOP_RANGE - TCP_DOWN_RANGE ; i++) {
       if (openTCPs[i] == 0)
            return TCP_DOWN_RANGE + i;            
    }
    return 0 ;
}


void setName (char* name){
    write(1 , "Enter your username : " , 22);
    char buff[1024] = {0};
    char input [128];
    int n =read(0 , input , 128);
    input[n - 1] = '\0';
    strcpy(name , input);
    sprintf(buff , "Welcome "YELLOW"%s" RESET_COLOR" as "PURPLE"Supplier\n"RESET_COLOR, name );
    write(1, buff ,strlen(buff));
}

void readAnswer(int rest_fd){
    char inputBuffer[16];
    memset(inputBuffer , 0 , 16);
    int n = read(1 , inputBuffer , sizeof(inputBuffer));
    inputBuffer[n-1] = '\0';
    while (n <= 0 && (strcmp(inputBuffer , "yes") || strcmp(inputBuffer , "no")) ){
        n = read(1 , inputBuffer , sizeof(inputBuffer));
        inputBuffer[n-1] = '\0';
        if (strcmp(inputBuffer , "yes") == 0){
            send(rest_fd , "yes" , strlen("yes") , 0 );
            break;
        }
        if (strcmp(inputBuffer , "no") == 0){
            send(rest_fd , "no" , strlen("no") , 0 );
            break;
        }
        
    }


}


int main(int argc, char const *argv[]) {
    int TCPport = firstAllowableTCPPort() ;
    


    Supplier* supplier = setupServer(TCPport);
    int new_socket, max_sd;
    char buffer[1024] = {0};
    fd_set master_set, working_set;


    FD_ZERO(&master_set);
    max_sd = supplier->sock;
    FD_SET(supplier->sock, &master_set);

    setName(supplier->name);

    int terminal_fd = fileno(stdin);
    fcntl(terminal_fd, F_SETFL, fcntl(terminal_fd, F_GETFL) | O_NONBLOCK);
    FD_SET(terminal_fd, &master_set);
    if (terminal_fd > max_sd) {
        max_sd = terminal_fd;
    }
    char inputBuffer[1024];
    int rest_fd = -1 ;




    while (1) {

        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);
        memset(inputBuffer , 0 , sizeof(inputBuffer));

        if (FD_ISSET(terminal_fd, &working_set)) {
            ssize_t bytes_read = read(terminal_fd, inputBuffer, sizeof(inputBuffer));
            if (bytes_read > 0) {
                inputBuffer[bytes_read - 1] = '\0'; // Remove newline
            }
            if (strcmp(inputBuffer , "answer request") == 0 ){
                sprintf(buffer , "Your answer is :("GREEN"yes"RESET_COLOR"/"RED"no"RESET_COLOR")\n");
                write(1 , buffer , strlen(buffer));
                readAnswer(rest_fd);
                close(rest_fd);
                continue;

            }
            
            
        }
        
        for (int i = 0; i <= max_sd; i++) {
            memset(buffer , 0 , 1024);
            if (FD_ISSET(i, &working_set)) {
                
                if (i == supplier->sock) {  // new clinet
                    new_socket = acceptClient(supplier->sock);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                }

                
                else { // client sending msg
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);
                    
                    if (strcmp(buffer , WHATISYOURNAME) == 0){
                        send(i , supplier->name , strlen(supplier->name) , 0);
                        memset(buffer , 0 , 1024);
                    }
                    if (strcmp(buffer , IF_YOU_ARE_RESTAURANT_SAY_YOUR_NAME) == 0){
                        close(i);
                        memset(buffer , 0 , 1024);
                    }
                    if (strcasecmp(buffer , REQUEST) == 0 ){
                        sprintf(buffer, GREEN"NEW"RESET_COLOR " Ingredient Request !\n" );
                        rest_fd = i ;
                    }
                        
                    
                    if (bytes_received == 0) { // EOF
                        close(i);
                        FD_CLR(i, &master_set);
                        continue;
                    }
                    write(1 , buffer , 1024);
                    memset(buffer, 0, 1024);
                }
            }
        }
        
        

    }

    return 0;
}
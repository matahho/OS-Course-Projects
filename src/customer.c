


#include "recipes.h"
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
#define CUSTOMERJOB 'C'
#define REQUEST "$request$"




#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define PURPLE  "\x1b[35m"
#define RESET_COLOR   "\x1b[0m"


typedef struct customerState { 
    char job ;
    bool isBusy ;
    char name [128];
    int BCPort ;
    int sock ;
    struct sockaddr_in bc_address;
    Food foods[MAX_FOOD_ITEMS];

    int tcpSock;
    struct sockaddr_in server_address;
    int tcpPort;

}Customer;





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
    sprintf(buff , "Welcome "YELLOW"%s" RESET_COLOR" as "GREEN"Customer\n"RESET_COLOR, name );
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



void AvailableRestaurants() {

    int *found = AllAvailablesTCP();
    int socket_desc;
    struct sockaddr_in server;
    char response[1024];
    char buff[4096]; // Increase the buffer size to hold all responses
    memset(buff, 0, sizeof(buff));

    for (int i = 0; i < TCP_TOP_RANGE - TCP_DOWN_RANGE; i++) {
        if (found[i] == 0) {
            continue;
        }

        socket_desc = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_desc == -1) {
            perror("Could not create socket");
            continue;
        }

        server.sin_family = AF_INET;
        server.sin_addr.s_addr = inet_addr("127.0.0.1");
        server.sin_port = htons(found[i]);

        if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) >= 0) {
            
            if (send(socket_desc, IF_YOU_ARE_RESTAURANT_SAY_YOUR_NAME, strlen(IF_YOU_ARE_RESTAURANT_SAY_YOUR_NAME), 0) < 0) {
                perror("Failed to send message");
                close(socket_desc);
                continue;
            }

            if (recv(socket_desc, response, sizeof(response), 0) < 0) {
                perror("Failed to receive response");
            } else {
                
                sprintf(buff, "%s"PURPLE"%s"RESET_COLOR" -> "BLUE"%d"RESET_COLOR"\n", buff, response, found[i]);
            }
        } else {
            perror("Connection failed");
        }

        close(socket_desc);
    }

    write(1, buff, strlen(buff));
}


Customer* setupServer(int port) {
    Customer *cust = (Customer*)malloc(sizeof(Customer));
    cust->isBusy = 0 ;
    cust->job = CUSTOMERJOB;
    struct sockaddr_in address;
    
    cust->sock = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(cust->sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(port);

    bind(cust->sock, (struct sockaddr *)&address, sizeof(address));
    
    listen(cust->sock, 1024);

    return cust;
}


void assignFoodArrayToCustomer(Customer *c) {
    Food *foodItems = parseJson();
    int numberOfFood = numberOfFoods();
    for (int i = 0 ; i < numberOfFood ;i++) {
        Food f ;
        sprintf(f.name , parseJson()[i].name , MAX_LEN_NAME );
        for (int k =0 ; k < MAX_INGREDIENT_NUMBER ; k++){
            f.ingredients[k] = parseJson()[i].ingredients[k];
        }
        f.ingredientCount =  parseJson()[i].ingredientCount;

        c->foods[i] = f;
    }
    
}

void showMenu(Customer *c){
    int n = numberOfFoods();
    
    char buff[256];
    for (int i = 0 ; i < n ; i++){
        memset(buff , 0 , sizeof(buff));
        sprintf(buff , "%d - %s\n" , i+1 , c->foods[i].name );
        write(1 , buff , strlen(buff));
        
    }
}



int main(int argc, char const *argv[]) {
    Customer* customer = setupServer(atoi(argv[1]));
    setName(customer->name);
    assignFoodArrayToCustomer(customer);
    int new_socket, max_sd;
    char buffer[1024] = {0};
    fd_set master_set, working_set;
    max_sd = (customer->sock > customer->tcpSock) ? customer->sock : customer->tcpSock;

    FD_ZERO(&master_set);
    FD_SET(customer->sock, &master_set);
    FD_SET(customer->tcpSock, &master_set);

    int terminal_fd = fileno(stdin);
    fcntl(terminal_fd, F_SETFL, fcntl(terminal_fd, F_GETFL) | O_NONBLOCK);
    FD_SET(terminal_fd, &master_set);
    if (terminal_fd > max_sd) {
        max_sd = terminal_fd;
    }

    char inputBuffer[1024] = {0};





    while (1) {

        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);
        memset(inputBuffer , 0 , sizeof(inputBuffer));

        if (FD_ISSET(terminal_fd, &working_set)) {
            ssize_t bytes_read = read(terminal_fd, inputBuffer, sizeof(inputBuffer));
            if (bytes_read > 0) {
                inputBuffer[bytes_read - 1] = '\0'; // Remove newline
            }
            if (strcmp(inputBuffer , "show menu") == 0 )
                showMenu(customer);
            if (strcmp(inputBuffer , "show restaurants") == 0 )
                AvailableRestaurants();
            continue;                
        }

        if (FD_ISSET( customer->sock , &working_set)){ //broadcast
            memset(buffer, 0, 1024);
            recv(customer->sock, buffer, 1024, 0);
            printf("%s\n", buffer);
            continue;
        }

        
        for (int i = 0; i <= max_sd; i++) {
            memset(buffer , 0 , 1024);
            if (FD_ISSET(i, &working_set)) {
                
                if (i == customer->tcpSock) {  // new clinet
                    new_socket = acceptClient(customer->tcpSock);
                    FD_SET(new_socket, &master_set);
                    if (new_socket > max_sd)
                        max_sd = new_socket;
                }

                
                
                else { // client sending msg
                    int bytes_received;
                    bytes_received = recv(i , buffer, 1024, 0);
                    
                    if (strcmp(buffer , WHATISYOURNAME) == 0)
                        close(i);
                    if (strcasecmp(buffer , REQUEST) == 0 )
                        close(i);
                    //if (strcasecmp(buffer , IF_YOU_ARE_RESTAURANT_SAY_YOUR_NAME) == 0 )
                        
                    
                    
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
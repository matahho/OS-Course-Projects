#include "recipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <sys/time.h>
#include <errno.h>


#define RESTAURANTJOB 'R'

#define TCP_TOP_RANGE 9090
#define TCP_DOWN_RANGE 5050



#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define PURPLE  "\x1b[35m"
#define RESET_COLOR   "\x1b[0m"


#define BUFFERLEN 1024


#define WHATISYOURNAME "$whatIsYourName$"
#define IF_YOU_ARE_RESTAURANT_SAY_YOUR_NAME "$IF_RESTAURANT_NAME$"
#define REQUEST "$request$"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <cjson/cJSON.h> 


typedef struct restaurantStates { 
    char job ;
    bool isOpen ;
    char name [128];
    int BCPort ;
    int sock ;
    struct sockaddr_in bc_address;
    Food foods[MAX_FOOD_ITEMS];
    Ingredient availables [MAX_INGREDIENTS];

    int tcpSock;
    struct sockaddr_in server_address;
    int tcpPort;
}Restaurant;

typedef struct reqIngredientInfo {
    int port;
    char name[BUFFERLEN/4];
    int amount;
}ReqIngredientInfo;





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
struct restaurantStates* setupServer(int port) {
    struct restaurantStates *restaurant = (struct restaurantStates*)malloc(sizeof(struct restaurantStates));
    restaurant->job = RESTAURANTJOB ;
    restaurant->isOpen = false;
    restaurant->BCPort = port;
    restaurant->sock = socket(AF_INET, SOCK_DGRAM, 0);

    int broadcast = 1, opt = 1;
    setsockopt(restaurant->sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(restaurant->sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    
    restaurant->bc_address.sin_family = AF_INET;
    restaurant->bc_address.sin_port = htons(port); 
    restaurant->bc_address.sin_addr.s_addr = inet_addr("192.168.0.156");



    bind(restaurant->sock, (struct sockaddr *)&restaurant->bc_address, sizeof(restaurant->bc_address));
    

    restaurant->tcpSock = socket(AF_INET, SOCK_STREAM, 0);
    restaurant->server_address.sin_family = AF_INET;
    restaurant->tcpPort = firstAllowableTCPPort();
    restaurant->server_address.sin_port = htons(restaurant->tcpPort); 

    restaurant->server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(restaurant->sock, (struct sockaddr *)&restaurant->bc_address, sizeof(restaurant->bc_address));

    return restaurant;
}


// int acceptClient(int server_fd) {
//     int client_fd;
//     struct sockaddr_in client_address;
//     int address_len = sizeof(client_address);
    
//     client_fd = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t*) &address_len);
//     //printf("Client connected!\n");

//     return client_fd;
// }

void AvailableSuppliers() {
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
            
            if (send(socket_desc, WHATISYOURNAME, strlen(WHATISYOURNAME), 0) < 0) {
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
void setName (char* name){
    write(1 , "Enter your username : " , 22);
    char buff[1024] = {0};
    char input [128];
    int n = read(0 , input , 128);
    //sprintf(input , "mahdi");
    input[n - 1] = '\0';
    strcpy(name , input);
    sprintf(buff , "Welcome "YELLOW"%s" RESET_COLOR" as "RED"Restaurant\n"RESET_COLOR, name );
    write(1, buff ,strlen(buff));
}



void broadcastMessage(const char* message , int sock ,struct sockaddr_in bc_address ){
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "%s", message);

    int a = sendto(sock, buffer, strlen(buffer), 0,(struct sockaddr *)&bc_address, sizeof(bc_address));
    
}

void openRestaurant (struct restaurantStates* rest){
    
    char buff[1024];
    rest->isOpen = true;

    memset(buff , 0 , BUFFERLEN);
    sprintf(buff , BLUE"%s"RESET_COLOR" Resturant is "GREEN"OPEN"RESET_COLOR" now !\n" , rest->name);
    broadcastMessage(buff , rest->sock , rest->bc_address);
    

}


void closeRestaurant (struct restaurantStates* rest){
    char buff[1024];
    rest->isOpen = false;
    memset(buff , 0 , BUFFERLEN);
    sprintf(buff , BLUE"%s"RESET_COLOR" Resturant is "RED"CLOSED"RESET_COLOR" now !\n" , rest->name);
    broadcastMessage(buff , rest->sock , rest->bc_address);


}

void listIngredients (struct restaurantStates* rest){
    char buff [1024];
    memset(buff , 0 , BUFFERLEN);
    sprintf(buff , "%s- - - - - - - - - -\n" ,buff);
    sprintf(buff , "%s%s" , buff, "Name / Amount\n\n");

    for (int i=0 ; i < MAX_INGREDIENTS ; i++){
        if (rest->availables[i].name[0] != '\0')
            sprintf(buff , "%s%s  %d\n" , buff , rest->availables[i].name , rest->availables[i].quantity);
    }
    sprintf(buff , "%s- - - - - - - - - -\n" , buff);
    write(1, buff ,strlen(buff));

}

ReqIngredientInfo* getRequestIngredient (){

    ReqIngredientInfo* ing = (ReqIngredientInfo*)malloc(sizeof(ReqIngredientInfo));


    char buff [BUFFERLEN];
    memset(buff , 0 , BUFFERLEN);

    sprintf(buff , "> Supplier's "BLUE"Port : "RESET_COLOR);
    write(1 , buff , strlen(buff));
    memset(buff , 0 , BUFFERLEN);
    
    
    int n = read(0 , buff , BUFFERLEN);
    while(n <= 0)
        n = read(0 , buff , BUFFERLEN);
    ing->port = atoi(buff);
    memset(buff , 0 , BUFFERLEN);
    


    sprintf(buff , "> Ingredient's "YELLOW"Name : "RESET_COLOR);
    write(1 , buff , strlen(buff));
    memset(buff , 0 , BUFFERLEN);

    
    n = read(0 , buff , BUFFERLEN);
    while (n <= 0 )
        n = read(0 , buff , BUFFERLEN);
    buff[n-1] = '\0';
    sprintf(ing->name ,"%s" ,buff );


    sprintf(buff , "> Ingredient's "YELLOW"Amount : "RESET_COLOR);
    write(1 , buff , strlen(buff));
    memset(buff , 0 , BUFFERLEN);


    n = read(0 , buff , BUFFERLEN);
    while (n <=0 )
        n = read(0 , buff , BUFFERLEN);
    ing->amount = atoi(buff);
    memset(buff , 0 , BUFFERLEN);

    return ing;
}
void increaseIngredient (Restaurant* rest , char *ingName , int amount){
    for (int i = 0 ; i < MAX_INGREDIENTS ; i++){
        if (strcmp(rest->availables[i].name , ingName) == 0 ){
            rest->availables[i].quantity += amount ;
            return;
        }
    }
    for (int i = 0 ; i < MAX_INGREDIENTS ; i++){
        if (rest->availables[i].name[0] == '\0') {
            strcpy(rest->availables[i].name, ingName);
            rest->availables[i].quantity = amount;
            return;
        }
    }

}

void sendRequestIngredient (Restaurant* rest , ReqIngredientInfo *ing){

    int socket_desc;
    struct sockaddr_in server;
    char response[1024];
    memset(response , 0 , sizeof(response));
    
    char buff[BUFFERLEN];
    memset(buff, 0, sizeof(buff));



    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) 
        perror("Could not create socket");
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(ing->port);

    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) >= 0) {
        
        if (send(socket_desc, REQUEST , strlen(REQUEST), 0) < 0) {
            perror("Failed to send message");
            close(socket_desc);
        }

        memset(buff , 0 , sizeof(buff));
        int n = recv(socket_desc, response, sizeof(response), 0);
        sprintf(buff, "%s", response);
            
        
    } else {
        perror("Connection failed");
    }

    close(socket_desc);
    
    if (strcmp(buff , "no")== 0){
        sprintf(buff , "The request "RED"DENIED"RESET_COLOR" from supplier "RED":("RESET_COLOR);
        return ;
    }if (strcmp(buff , "yes")== 0){
        sprintf(buff , "The request "GREEN"ACCEPTED"RESET_COLOR" from supplier "GREEN":)"RESET_COLOR);
        increaseIngredient(rest , ing->name , ing->amount);
        return ;
    }
    
    
}



void assignFoodArrayToRestaurant(Restaurant *restaurant) {
    Food *foodItems = parseJson();
    int numberOfFood = numberOfFoods();
    for (int i = 0 ; i < numberOfFood ;i++) {
        Food f ;
        sprintf(f.name , parseJson()[i].name , MAX_LEN_NAME );
        for (int k =0 ; k < MAX_INGREDIENT_NUMBER ; k++){
            f.ingredients[k] = parseJson()[i].ingredients[k];
        }
        f.ingredientCount =  parseJson()[i].ingredientCount;

        restaurant->foods[i] = f;
    }
    
}

void assignIngredientsArrayToRestaurant(Restaurant *restaurant) {
    int n = numberOfFoods();
    Ingredient ava[100] ;

    int count = 0 ;
    for (int i =0 ; i < n ; i++){
        for (int j=0 ; j < restaurant->foods[i].ingredientCount ; j++){
            int flag =0;
            for (int k =0 ; k < count ; k++){
                if (strcmp(ava[k].name, restaurant->foods[i].ingredients[j].name ) == 0 ){
                    flag = 1;
                    break;
                }
            }
            if(flag == 0 ){
                
                strcpy(ava[count].name , restaurant->foods[i].ingredients[j].name );
                ava[count].quantity = 0 ;
                count += 1 ;
            }
        }
    }


    for( int i = 0 ; i <count ; i++)
        restaurant->availables[i] = ava[i];
}


void listRecipes(Restaurant* rest){
    int n = numberOfFoods();
    char buff[256];
    for (int i = 0 ; i < n ; i++){
        memset(buff , 0 , sizeof(buff));
        sprintf(buff , "%d - %s\n" , i+1 , rest->foods[i].name );
        write(1 , buff , strlen(buff));
        for (int j =0 ; j < rest->foods[i].ingredientCount ; j++){
            memset(buff , 0 , sizeof(buff));
            sprintf(buff , "\t%s - %d\n" , rest->foods[i].ingredients[j].name , rest->foods[i].ingredients[j].quantity);
            write(1 , buff , strlen(buff));
        }

    }
}


int main(int argc, char const *argv[]) {
    char buff[BUFFERLEN] = {0};
    Restaurant* restaurant = setupServer(atoi(argv[1]));
    assignFoodArrayToRestaurant(restaurant);
    assignIngredientsArrayToRestaurant(restaurant);    
    setName(restaurant->name);
    openRestaurant(restaurant);

    int new_socket, max_sd;
    char buffer[1024] = {0};
    fd_set master_set, working_set;
    max_sd = (restaurant->sock > restaurant->tcpSock) ? restaurant->sock : restaurant->tcpSock;

    FD_ZERO(&master_set);
    FD_SET(restaurant->sock, &master_set);
    FD_SET(restaurant->tcpSock, &master_set);

    int terminal_fd = fileno(stdin);
    fcntl(terminal_fd, F_SETFL, fcntl(terminal_fd, F_GETFL) | O_NONBLOCK);
    FD_SET(terminal_fd, &master_set);
    if (terminal_fd > max_sd) {
        max_sd = terminal_fd;
    }


    


    while (1) {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);
        memset(buff , 0 , BUFFERLEN);
        
        
        if (FD_ISSET(terminal_fd, &working_set)) {
            int n = read(1 , buff , BUFFERLEN);
            if (n > 0) {
                buff[n - 1] = '\0'; 
            }
            if (strcmp(buff , "break") == 0 ){
                if (restaurant->isOpen)
                    closeRestaurant(restaurant);
                else {
                    memset(buff , 0 , BUFFERLEN);
                    sprintf(buff , "%s Was "RED"CLOSED !\n"RESET_COLOR , restaurant->name );
                    write(0 ,buff , BUFFERLEN);
                }

            }

            if (strcmp(buff , "start working") == 0){
                if (restaurant->isOpen){
                    memset(buff , 0 , BUFFERLEN);
                    sprintf(buff , "%s Was "GREEN"OPEN !\n"RESET_COLOR , restaurant->name );
                    write(0 ,buff , BUFFERLEN);
                    
                }else
                    openRestaurant(restaurant);
            }

            if (strcmp(buff , "show ingredients") == 0)
                listIngredients(restaurant);
        
            if (strcmp(buff , "show suppliers") == 0 )
                AvailableSuppliers();
            
            if (strcmp(buff , "request ingredient") == 0 ){
                ReqIngredientInfo* ing = getRequestIngredient();
                sendRequestIngredient(restaurant , ing);
                
            }
            if (strcmp(buff , "show recipes") == 0 )
                listRecipes(restaurant);
            
            

        }

        
       

        
        
        
        
    }




    close(restaurant->sock);
   

    return 0;
}
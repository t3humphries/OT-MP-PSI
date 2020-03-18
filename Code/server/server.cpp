#include "server.h"
#include "Keyholder.h"
#define PORT 8080 
#define TRUE   1 

int server_fd, new_socket, valread; 
struct sockaddr_in address;
char buffer[1024] = {0}; 
char *message = "message/ack from server";   //value that you want to send back to client

int serverInitFunction() 
{ 
     
    int opt = 1; 
    int addrlen = sizeof(address); 
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
   
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0) 
    { 
        perror("The binding failed"); 
        exit(EXIT_FAILURE); 
    } 
   
    if (listen(server_fd, 4) < 0)   // The second parameter is the length of the queue for pending connections. can be changed if necessary.
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }
    
    return 0; 
} 

int main()
{
    serverInitFunction();
    int addrlen = sizeof(address); 
    
    //The server runs in a loop below. It waits for requests on the file descriptor and accepts the first pending request for the listening socket.
    
    while(TRUE)
    {    
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        } 
        valread = read( new_socket , buffer, 1024); 
        printf("%s\n",buffer );                   // this is the value from the client. perform your operations on this
        send(new_socket , message , strlen(message) , 0 ); 
    } 

    return 0;
}

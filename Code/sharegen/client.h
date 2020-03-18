#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h> 
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>

#define PORT 8080 
#define size 1024

class client{

public:

    int sock, valread;
    struct sockaddr_in serv_addr; 
    char buffer[size];

    client(char *connection_address);   
    char* clientfunction(char* message);
    
};

#endif
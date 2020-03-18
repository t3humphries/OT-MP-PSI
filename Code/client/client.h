#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h> 
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <vector>
using namespace std;

#define PORT 8080 

class client{

public:

    int sock;
    struct sockaddr_in serv_addr; 
    unsigned int MAX_BUF_LENGTH;

    client(string connection_address);   
    string send_to_server(string arg, string message);
    
};

#endif
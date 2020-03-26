#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h> 
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <vector>
#include <fstream>

using namespace std;

#define PORT 8080 

class client{

public:

    int sock;
    struct sockaddr_in serv_addr; 
    unsigned int MAX_BUF_LENGTH;
    int log;
    std::vector<int> s1r1, s1r2, s2;

    client(string connection_address, int log_sizes);
    client(){} 
    //client(const client &old_obj); 
    string send_to_server(string arg, string message);
    string get_message_sizes();
    
};

#endif
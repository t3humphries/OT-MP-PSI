#include "server.h"
#include "Keyholder.h"
#define PORT 8080 
#define TRUE   1 

int server_fd, new_socket; 
struct sockaddr_in address;
unsigned int MAX_BUF_LENGTH = 4096;

void serverInitFunction() 
{ 
    int opt = 1; 
    
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

        //Get the input from the client
        vector<char> buffer(MAX_BUF_LENGTH);
        string input_from_client;   
        int bytesReceived = 0;
        do {
            bytesReceived = read(new_socket, &buffer[0], buffer.size());
            // append string from buffer.
            if ( bytesReceived == -1 ) { 
                // error 
            } else {
                input_from_client.append( buffer.cbegin(), buffer.cend() );
            }
        } while ( bytesReceived == MAX_BUF_LENGTH );

        cout<<input_from_client<<endl;  

        string message = "Testing response";      
        send(new_socket , message.c_str() , message.length() , 0 ); 
    } 

    return 0;
}
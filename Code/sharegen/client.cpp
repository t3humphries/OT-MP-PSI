

#include "client.h" 

using namespace std;


client::client(char *connection_address)   // constructor for intiializing
{
    sock= 0;
    for (int i=0;i<size;i++)
    {
        buffer[i] = 0;
    }        
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT);                 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
       // return -1; 
    } 
}

       
char* client::clientfunction(char* message) 
{ 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
    }
     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        return "\nConnection failed"; 
    } 
    send(sock , message , strlen(message) , 0 ); 
    valread = read( sock , buffer, 1024); 
    if(close(sock)<0)
    {
        cout<<"connection could not be closed";
    } 

    return buffer; 
} 



int main()
{
    client object("127.0.0.1");
	cout<<"first return"<<object.clientfunction("this is the message one")<<"\n";     //These are test messages.
      
	return 0;
}


#include "client.h" 
using namespace std;


client::client(string connection_address)   // constructor for intiializing
{
    sock= 0;  
    MAX_BUF_LENGTH = 6096;  
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT);                 
    if(inet_pton(AF_INET, connection_address.c_str(), &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
    } 
}

       
string client::send_to_server(string arg, string raw_message) 
{ 

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
    }
     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        return "\nConnection failed"; 
    }
    //Send to server 
    string message = arg + "|" + raw_message;
    send(sock , message.c_str() , message.length() , 0 ); 

    //Get the response from server
    vector<char> buffer(MAX_BUF_LENGTH);
    string response;   
    int bytesReceived = 0;
    do {
        bytesReceived = read(sock, &buffer[0], buffer.size());
        // append string from buffer.
        if ( bytesReceived == -1 ) { 
            // error 
        } else {
            response.append( buffer.cbegin(), buffer.cend() );
        }
    } while ( bytesReceived == MAX_BUF_LENGTH );

    if(close(sock)<0)
    {
        cout<<"connection could not be closed";
    } 
    return response; 
} 



// int main()
// {
//     client object("127.0.0.1");
// 	cout<<"first return"<<object.send_to_server("this is the message one")<<"\n";    
//     cout<<"first return"<<object.send_to_server("this is the message two")<<"\n";  //These are test messages.
      
// 	return 0;
// }

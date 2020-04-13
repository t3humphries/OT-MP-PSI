
#include "client.h" 
using namespace std;


client::client(string connection_address, int __log)   // constructor for intiializing
{
    sock= 0;  
    MAX_BUF_LENGTH = 4096;  
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT);                 
    if(inet_pton(AF_INET, connection_address.c_str(), &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
    }

    log = __log;
    
    for(int i=0 ; i<3 ;i++)
    {
        s1r1.push_back(0);
        s1r2.push_back(0);
        s2.push_back(0);
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
        buffer = vector<char>(MAX_BUF_LENGTH);
        bytesReceived = read(sock, &buffer[0], buffer.size());
        // append string from buffer.
        if ( bytesReceived == -1 ) { 
            // error 
        } else {
            response.append( buffer.cbegin(), buffer.cend() );
        }
    } while ( bytesReceived == MAX_BUF_LENGTH );
    response.resize(strlen(response.c_str()));
    if(close(sock)<0)
    {
        cout<<"connection could not be closed";
    } 

    
    if(arg.compare("S1_R1") == 0)
    {
        s1r1[0] += 1;
        s1r1[1] += message.length();
        s1r1[2] += response.length();
    }
    
    else if(arg.compare("S1_R2") == 0)
    {
        s1r2[0] += 1;
        s1r2[1] += message.length();
        s1r2[2] += response.length();
    }

    else if(arg.compare("S2") == 0)
    {
        s2[0] += 1;
        s2[1] += message.length();
        s2[2] += response.length();
        }
    
    return response; 
} 

vector <int> client::get_message_sizes()   //here
{
  // string toReturn = "Message Sizes: \n";
    vector<int>  toReturn;
   if(s1r1[0] != 0)
   {
     toReturn.push_back(s1r1[1]/s1r1[0]);
     toReturn.push_back(s1r1[2]/s1r1[0]);
   }
   if(s1r2[0] != 0)
   {
    toReturn.push_back(s1r2[1]/s1r2[0]);
    toReturn.push_back(s1r2[2]/s1r2[0]);
   }
   if(s2[0] != 0)
   {
    toReturn.push_back(s2[1]/s2[0]);
    toReturn.push_back(s2[2]/s2[0]);
   }
   return toReturn;
}



// int main()
// {
//     client object("127.0.0.1");
// 	cout<<"first return"<<object.send_to_server("this is the message one")<<"\n";    
//     cout<<"first return"<<object.send_to_server("this is the message two")<<"\n";  //These are test messages.
      
// 	return 0;
// }

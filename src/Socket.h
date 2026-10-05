#ifndef SOCKET_H
#define SOCKET_H

#include <string>

class Socket { 
    int sockfd;

public:
    Socket (): sockfd(-1) {};
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket &&other);
    Socket& operator=(Socket &&other);      
    ~Socket() {Close();};                              
 
    Socket Accept ();
    bool Close ();    
    bool IsValid () const;
    bool Listen (int port);        
    bool RecvAll (char* buf, int n);      
    bool SendAll (const char* buf, int n);   
    bool Connect (std::string ip, int port); 
};

#endif
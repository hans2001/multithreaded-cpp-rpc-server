#ifndef CLIENTSTUB_H
#define CLIENTSTUB_H

#include "Message.h"
#include "Socket.h"
#include <string>

class ClientStub { 
    Socket socket;
    
public:
    bool Init(std::string ip, int port);
    bool OrderRequest(const Order &order, RobotInfo &robot);
    void Close();
};

#endif
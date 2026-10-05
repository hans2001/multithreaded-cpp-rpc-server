#ifndef SERVERSTUB_H
#define SERVERSTUB_H

#include "Message.h"
#include "Socket.h"

class ServerStub { 
    Socket socket;
    
public:
    bool Init(Socket &&s);
    bool ReceiveOrder(Order &order);
    bool ShipRobot(const RobotInfo &robot);
    void Close();
};

#endif
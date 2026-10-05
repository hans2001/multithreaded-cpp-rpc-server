#include <string>
#include "ClientStub.h"
#include "Message.h"


bool ClientStub::Init(std::string ip, int port) { 
    return socket.Connect(ip, port);
}

bool ClientStub::OrderRequest(const Order &order, RobotInfo &robot) {
    char buffer[ROBOTINFO_BUF_SIZE];  
    int n = order.Marshal(buffer);   
    if (!socket.SendAll(buffer, n)) return false;
    if (!socket.RecvAll(buffer, ROBOTINFO_BUF_SIZE)) return false;
    robot.Unmarshal(buffer);
    return true;
}

void ClientStub::Close() {
    socket.Close();
}
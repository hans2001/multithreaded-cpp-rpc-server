#include "ServerStub.h"
#include "Message.h"
#include <utility>

bool ServerStub::Init(Socket &&s) { 
    socket = std::move(s);  
    return socket.IsValid();
}

bool ServerStub::ReceiveOrder(Order &order) {
    char buffer[ORDER_BUF_SIZE];
    if (!socket.RecvAll(buffer, ORDER_BUF_SIZE)) {
        return false;
    } 
    order.Unmarshal(buffer);
    return true;
}

bool ServerStub::ShipRobot(const RobotInfo &robot) {
    char buffer[ROBOTINFO_BUF_SIZE];
    int n = robot.Marshal(buffer);
    return socket.SendAll(buffer, n);
}

void ServerStub::Close() {
    socket.Close();
}
#include <iostream>

#include <string.h>
#include <arpa/inet.h>

#include "Message.h"

void Order::SetOrder(int cust_id, int order_num, int robot_tp) {
    customer_id = cust_id;
    order_number = order_num;
    robot_type = robot_tp;
}

void Order::PrintOrder() const {
    std::cout << "\tOrder - customer_id: " << customer_id;
    std::cout << "; order_number: " << order_number << "; ";
    std::cout << "robot_type: " << robot_type << std::endl;
}

int Order::Marshal(char *buffer) const {
    int offset = 0;

    uint32_t net_customer_id = htonl(customer_id);
    memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);        

    uint32_t net_order_number = htonl(order_number);
    memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
    offset += sizeof(net_order_number);        

    uint32_t net_robot_type = htonl(robot_type);
    memcpy(buffer + offset, &net_robot_type, sizeof(net_robot_type));
    offset += sizeof(net_robot_type); 

    return offset;
}

void Order::Unmarshal(char *buffer) {
    int offset = 0;

    uint32_t net_customer_id;                                  
    memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id)); 
    customer_id = ntohl(net_customer_id);                               
    offset += sizeof(net_customer_id);

    uint32_t net_order_number;                                  
    memcpy(&net_order_number, buffer + offset, sizeof(net_order_number)); 
    order_number = ntohl(net_order_number);                               
    offset += sizeof(net_order_number);

    uint32_t net_robot_type;                                  
    memcpy(&net_robot_type, buffer + offset, sizeof(net_robot_type)); 
    robot_type = ntohl(net_robot_type);                               
    offset += sizeof(net_robot_type);
}

int RobotInfo::Marshal(char *buffer) const { 
    int offset = 0;

    uint32_t net_customer_id = htonl(customer_id);
    memcpy(buffer + offset, &net_customer_id, sizeof(net_customer_id));
    offset += sizeof(net_customer_id);        

    uint32_t net_order_number = htonl(order_number);
    memcpy(buffer + offset, &net_order_number, sizeof(net_order_number));
    offset += sizeof(net_order_number);        

    uint32_t net_robot_type = htonl(robot_type);
    memcpy(buffer + offset, &net_robot_type, sizeof(net_robot_type));
    offset += sizeof(net_robot_type); 

    uint32_t net_engineer_id = htonl(engineer_id);
    memcpy(buffer + offset, &net_engineer_id, sizeof(net_engineer_id));
    offset += sizeof(net_engineer_id);        

    uint32_t net_expert_id = htonl(expert_id);
    memcpy(buffer + offset, &net_expert_id, sizeof(net_expert_id));
    offset += sizeof(net_expert_id); 

    return offset;
}

void RobotInfo::Unmarshal(char *buffer) {
    int offset = 0;

    uint32_t net_customer_id;                                  
    memcpy(&net_customer_id, buffer + offset, sizeof(net_customer_id)); 
    customer_id = ntohl(net_customer_id);                               
    offset += sizeof(net_customer_id);

    uint32_t net_order_number;                                  
    memcpy(&net_order_number, buffer + offset, sizeof(net_order_number)); 
    order_number = ntohl(net_order_number);                               
    offset += sizeof(net_order_number);

    uint32_t net_robot_type;                                  
    memcpy(&net_robot_type, buffer + offset, sizeof(net_robot_type)); 
    robot_type = ntohl(net_robot_type);                               
    offset += sizeof(net_robot_type);
    
    uint32_t net_engineer_id;                                  
    memcpy(&net_engineer_id, buffer + offset, sizeof(net_engineer_id)); 
    engineer_id = ntohl(net_engineer_id);                               
    offset += sizeof(net_engineer_id);

    uint32_t net_expert_id;                                  
    memcpy(&net_expert_id, buffer + offset, sizeof(net_expert_id)); 
    expert_id = ntohl(net_expert_id);                               
    offset += sizeof(net_expert_id);
};

void RobotInfo::SetEngineerID(int id) {engineer_id = id;}
void RobotInfo::SetExpertID(int id) {expert_id = id;}

void RobotInfo::CopyOrder(const Order& order) {
    customer_id = order.GetCustomerID();
    order_number = order.GetOrderNumber();
    robot_type = order.GetRobotType();
}

void RobotInfo::PrintRobotInfo() const {
    std::cout << "\tRobotInfo - customer_id: " << customer_id << "; order_number: " << order_number;
    std::cout << "; robot_type: " << robot_type << "; engineer_id: " << engineer_id << "; expert_id: " << expert_id <<  std::endl;
}

#ifndef MESSAGE_H
#define MESSAGE_H

#define ORDER_BUF_SIZE       12
#define ROBOTINFO_BUF_SIZE   20

class Order {
    int customer_id;         
    int order_number;      
    int robot_type;   

public:
    Order(): customer_id(0), order_number(0), robot_type(0) {}

    int GetCustomerID() const { return customer_id; }
    int GetOrderNumber() const { return order_number; }
    int GetRobotType() const { return robot_type; }

    void SetOrder(int cust_id, int order_num, int robot_tp);
    int Marshal(char *buffer) const;
    void Unmarshal(char *buffer);
    void PrintOrder() const;
};

class RobotInfo {
    int customer_id;
    int order_number;
    int robot_type;
    int engineer_id;
    int expert_id;

public:
    RobotInfo(): customer_id(0), order_number(0), robot_type(0), engineer_id(-1), expert_id(-1) {}

    int GetCustomerID() const { return customer_id; }
    int GetOrderNumber() const { return order_number; }
    int GetRobotType() const { return robot_type; }
    int GetEngineerID() const { return engineer_id; }
    int GetExpertID() const { return expert_id; }
    
    void SetEngineerID(int id);
    void SetExpertID(int id);

    void CopyOrder(const Order&);
    int Marshal(char *buffer) const;
    void Unmarshal(char *buffer);
    void PrintRobotInfo() const;
};

#endif
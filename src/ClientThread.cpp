#include <algorithm>
#include <chrono>
#include <string>
#include "ClientThread.h"
#include "ClientStub.h"

void ClientThread(std::string ip, int port, int customer_id, int num_orders, int robot_type, CustomerStats* stats) { 
    ClientStub stub;
    if (!stub.Init(ip, port)){
        return;
    }

    for (int i = 0; i < num_orders; i++) {
        Order order;
        order.SetOrder(customer_id, i, robot_type);
        RobotInfo robot;
        
        const auto start = std::chrono::high_resolution_clock::now();
        bool sent = stub.OrderRequest(order, robot);
        const auto end = std::chrono::high_resolution_clock::now();
   
        if (!sent) {
            break;
        }

        const long long us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        stats->total_us += us;
        stats->count++;
		stats->min_us = std::min(stats->min_us, us); 
		stats->max_us = std::max(stats->max_us, us); 
    }

    stub.Close();
}
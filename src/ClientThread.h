#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <climits>
#include <string>

struct CustomerStats { 
    long long total_us = 0;
    long long min_us = LLONG_MAX;
    long long max_us = 0; 
    long long count = 0;
};

void ClientThread(std::string ip, int port, int customer_id, int num_orders, int robot_type, CustomerStats* stats);

#endif
#include <iostream>
#include <thread> 
#include <utility>
#include <cstdlib>
#include "ServerFactory.h"
#include "Socket.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [port #] [# experts]" << std::endl;
        return 1;
    }
    
    int port = atoi(argv[1]);
    if (port < 10000 || port > 65535) {
        std::cerr << "ERROR: port must be between 10000 and 65535" << std::endl;
        return 1;
    }

    Socket listener;
    ServerFactory factory;
    
    // Section 2 runs the server as "./server [port #]"; default to 1 expert in that case
    int num_experts = (argc >= 3) ? atoi(argv[2]) : 1;
    if (num_experts < 1) {
        std::cerr << "ERROR: number of experts must be bigger than or equal to 1" << std::endl;
        return 1;
    }

    factory.StartExperts(num_experts);

    if (!listener.Listen(port)) return 1;
    
    // engineer_id start from num_experts, so expert_id never overlaps with engineer_id
    int engineer_id = num_experts;
    while (true) {
        Socket conn = listener.Accept();
        if (!conn.IsValid()) continue;
        std::thread(&ServerFactory::EngineerThread, &factory, std::move(conn), engineer_id++).detach();
    }

    return 0;
}

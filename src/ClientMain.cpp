#include <iostream> 
#include <thread>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cstdlib>
#include <string>

#include "ClientThread.h"

int main(int argc, char *argv[]) {
	if (argc < 6) {
		std::cerr << "Usage: " << argv[0] << " [ip addr] [port #] [# customers] [# orders] [robot type]" << std::endl;
		return 1;
	}

	std::string ip = argv[1];
	int port = atoi(argv[2]); 
	int num_customers = atoi(argv[3]);
	int num_orders = atoi(argv[4]);
	int robot_type = atoi(argv[5]);

	if (port < 10000 || port > 65535) {
		std::cerr << "ERROR: port must be between 10000 and 65535" << std::endl;
		return 1;
	}

	if (num_customers <= 0 || num_orders <= 0) {
		std::cerr << "ERROR: # customers and # orders must be positive" << std::endl;
		return 1;
	}

	if (robot_type != 0 && robot_type != 1) {
		std::cerr << "ERROR: robot type must be 0 (regular) or 1 (special)" << std::endl;
		return 1;
	}

	std::vector<std::thread> threads;
	std::vector<CustomerStats> stats(num_customers);   

	const auto start = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < num_customers; i++) {
		std::thread t(ClientThread, ip, port, i, num_orders, robot_type, &stats[i]);
		threads.push_back(std::move(t));
    }

    for (auto& thread : threads) {
        thread.join(); 
    }

	const auto end = std::chrono::high_resolution_clock::now();

	long long total_us = 0, total_count = 0;
	long long min_us = LLONG_MAX, max_us = 0;
	
	for (const auto &s:stats) { 
		total_us += s.total_us;
        total_count += s.count;
		min_us = std::min(min_us, s.min_us); 
		max_us = std::max(max_us, s.max_us); 
	}

	if (total_count == 0) {
		std::cerr << "ERROR: no orders completed" << std::endl;
    	return 1;
	}

	double avg = (double)total_us / total_count;
	double seconds = std::chrono::duration<double>(end - start).count();
	double throughput = total_count / seconds;
	
	std::cout << avg << "\t" << min_us << "\t" << max_us << "\t" << throughput << std::endl;
	return 0;
}

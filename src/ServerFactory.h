#ifndef SERVERFACTORY_H
#define SERVERFACTORY_H

#include "Socket.h"
#include "Message.h"
#include <future>
#include <queue>
#include <mutex>
#include <condition_variable>

struct ExpertRequest {
    RobotInfo info;
    std::promise<RobotInfo> promise;
};

class ServerFactory  { 
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<ExpertRequest> jobQueue;
    
    RobotInfo CreateRegularRobot(const Order &order, int engineer_id);
    RobotInfo CreateSpecialRobot(const Order &order, int engineer_id);

public:
    void ExpertThread(int expert_id);   
    void StartExperts(int num_experts);
    void EngineerThread(Socket conn, int engineer_id);   
};

#endif
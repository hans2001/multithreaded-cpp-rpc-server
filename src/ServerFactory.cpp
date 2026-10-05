#include "ServerFactory.h"
#include "ServerStub.h"
#include "Message.h"
#include <chrono>
#include <thread>
#include <utility>
#include <mutex>

void ServerFactory::EngineerThread(Socket conn, int engineer_id) { 
    ServerStub stub;
    if (!stub.Init(std::move(conn))) return;
    
    Order order;
    while (stub.ReceiveOrder(order)) {
        RobotInfo robot;
        if (order.GetRobotType() == 0) {
           robot = CreateRegularRobot(order, engineer_id);
        } else { 
           robot = CreateSpecialRobot(order, engineer_id);
        }
        if (!stub.ShipRobot(robot)) break;
    }
    stub.Close();
}

RobotInfo ServerFactory::CreateRegularRobot(const Order &order, int engineer_id) {
    RobotInfo robot_info;
    robot_info.CopyOrder(order);
    robot_info.SetEngineerID(engineer_id);
    robot_info.SetExpertID(-1);
    return robot_info;
}

RobotInfo ServerFactory::CreateSpecialRobot(const Order &order, int engineer_id) {
    RobotInfo robot_info;
    robot_info.CopyOrder(order);
    robot_info.SetEngineerID(engineer_id);
    
    ExpertRequest req;
    req.info = std::move(robot_info);
    
    auto fut = req.promise.get_future();
    {
        std::unique_lock<std::mutex> lk(mtx);
        jobQueue.push(std::move(req));
        cv.notify_one();
    }
    
    return fut.get();
}

void ServerFactory::ExpertThread(int expert_id) {
    while(true) { 
        ExpertRequest req;
        { 
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk,[&] { return !jobQueue.empty(); });
            req = std::move(jobQueue.front());
            jobQueue.pop();
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        req.info.SetExpertID(expert_id);
        req.promise.set_value(req.info);
    }
}         

void ServerFactory::StartExperts(int num_experts) {
    for (int i = 0; i < num_experts; i++) {
        std::thread(&ServerFactory::ExpertThread, this, i).detach();
    }
}

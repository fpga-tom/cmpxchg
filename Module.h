//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_MODULE_H
#define CMPXCHG_MODULE_H

#include <cstdint>
#include <vector>
#include <memory>
#include <thread>
#include "Task.h"

class Module {
protected:
    Task& create_task(const std::string &name);
public:
    std::vector<std::shared_ptr<Task>> tasks;
    std::vector<std::thread> threads;
    Task& operator[](const int id);

    void start();
    void join();

};


#endif //CMPXCHG_MODULE_H

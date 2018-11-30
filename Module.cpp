//
// Created by tomas on 11/30/18.
//

#include <thread>
#include "Module.h"

Task& Module::create_task(const std::string &name) {
    auto t = std::make_shared<Task>(name);
    tasks.emplace_back(t);
    return *tasks.back();
}

Task &Module::operator[](const int id) {
    return *tasks[id];
}

void Module::start() {

    for(uint64_t i = 0; i < tasks.size(); i++) {
        threads.emplace_back(std::thread(tasks[i]->codelet));
    }

}

void Module::join() {
    for(uint64_t i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}

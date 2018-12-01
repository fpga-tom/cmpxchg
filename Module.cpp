//
// Created by tomas on 11/30/18.
//

#include <thread>
#include <iostream>
#include "Module.h"

Task& Module::create_task(const std::string &name, std::initializer_list<TagPort> ports, std::function<int(uint8_t*[], uint8_t*[])> codelet) {
    auto t = std::make_shared<Task>(name, ports, codelet);
    tasks.emplace_back(t);
    return *tasks.back();
}

Task &Module::operator[](const int id) {
    return *tasks[id];
}

void Module::start() {

    for(uint64_t i = 0; i < tasks.size(); i++) {
        threads.emplace_back(std::thread([this](uint64_t i) -> void {
            std::cout << "* Starting " << tasks[i]->name.c_str() << "..." << std::endl;
            pthread_setname_np(pthread_self(), tasks[i]->name.c_str());
            tasks[i]->exec();
        },i));
    }

}

void Module::join() {
    for(uint64_t i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
}

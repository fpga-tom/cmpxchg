//
// Created by tomas on 11/23/18.
//

#ifndef CMPXCHG_TASK_H
#define CMPXCHG_TASK_H


#include <cstdio>
#include <string>
#include <vector>
#include <memory>
#include <bits/shared_ptr.h>
#include <functional>
#include "Port.h"

class Module;
class Task;
class Task {
    friend Module;
    const std::string &name;
    std::vector<std::shared_ptr<Port>> ports;

    std::function<int(void)> codelet;


protected:
    Port& create_port(const std::string &name);

public:
    Task(const std::string &name);
    inline Port& operator[](const int id)
    {
        return *this->ports[id];
    }
    Port& create_port_in(const std::string &name);
    Port& create_port_out(const std::string &name, size_t n_elems, uint64_t n_buf = 2);
    void create_codelet(std::function<int(void)> codelet);
    std::vector<std::shared_ptr<std::vector<uint8_t >>> buf;
};


#endif //CMPXCHG_TASK_H

//
// Created by tomas on 11/23/18.
//

#include "Task.h"
Port &Task::create_port_in(const std::string &name) {
    return create_port(name);
}

Port &Task::create_port_out(const std::string &name, size_t n_elems, uint64_t n_buf) {
    Port& port = create_port(name);
    for(uint64_t i = 0; i < n_buf; i++) {
        buf.emplace_back(std::move(std::shared_ptr<std::vector<uint8_t >>(new std::vector<uint8_t >(n_elems))));
    }
    return port;
}

Port &Task::create_port(const std::string &name) {
    auto s = std::make_shared<Port>(name);

    ports.push_back(std::move(s));

    return *ports.back();
}

void Task::create_codelet(std::function<int(void)> codelet) {
    this->codelet = codelet;
}

Task::Task(const std::string &name) : name(name) {

}

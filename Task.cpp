//
// Created by tomas on 11/23/18.
//

#include "Task.h"
Port &Task::create_port_in(const std::string &name, const uint8_t id) {
    return create_port(name, id);
}

Port &Task::create_port_out(const std::string &name,const uint8_t id, size_t n_elems, uint64_t n_buf) {
    Port& port = create_port(name, id);
    for(uint64_t i = 0; i < n_buf; i++) {
        buf.emplace_back(std::move(std::shared_ptr<std::vector<uint8_t >>(new std::vector<uint8_t >(n_elems))));
    }
    return port;
}

Port &Task::create_port(const std::string &name, const uint8_t id) {
    auto s = std::make_shared<Port>(name);

    ports[id] = std::move(s);

    return *ports[id];
}

void Task::create_codelet(std::function<int(void)> codelet) {
    this->codelet = codelet;
}

Task::Task(const std::string &name, std::initializer_list<TagPort> ports, std::function<int(void)> codelet) : name(name), ports(ports.size()) {
    for(TagPort p : ports) {
        if(p.tag_type == TagPort::tag_type_t::port_in) {
            create_port_in(p.name, p.id);
        }
        if(p.tag_type == TagPort::tag_type_t::port_out) {
            create_port_out(p.name, p.id, p.size);
        }
    }
    create_codelet(codelet);

}

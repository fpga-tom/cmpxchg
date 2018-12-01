//
// Created by tomas on 11/23/18.
//

#include "Task.h"
Port &Task::create_port_in(const std::string &name, const uint8_t id) {
    Port &port = create_port(name, id);
    ports_in.emplace_back(ports[id]);
    return port;
}

Port &Task::create_port_out(const std::string &name,const uint8_t id, size_t n_elems, uint64_t n_buf) {
    Port& port = create_port(name, id);
    for(uint64_t i = 0; i < n_buf; i++) {
        buf.emplace_back(std::move(std::shared_ptr<std::vector<uint8_t >>(new std::vector<uint8_t >(n_elems))));
    }
    ports_out.emplace_back(ports[id]);
    return port;
}

Port &Task::create_port(const std::string &name, const uint8_t id) {
    auto s = std::make_shared<Port>(name);

    ports[id] = std::move(s);

    return *ports[id];
}

void Task::create_codelet(std::function<int(uint8_t*[], uint8_t*[])> codelet) {
    this->codelet = codelet;
}

void Task::exec() {

    for(uint64_t i= 0; true ;i++) {

        uint8_t *d_in[ports_in.size()];
        uint8_t *d_out[ports_out.size()];

        int c = 0;
        for (std::shared_ptr<Port> p : ports_in) {
            uint8_t *d = (uint8_t *) p->poll();
            d_in[c++] = d;
        }

        c = 0;
        for (std::shared_ptr<Port> p : ports_out) {
            uint8_t *d = (uint8_t *) buf[3 * c + i % 3]->data();
            d_out[c++] = d;
        }

        this->codelet(d_in, d_out);

        c = 0;
        for (std::shared_ptr<Port> p : ports_out) {
            p->put(reinterpret_cast<uint64_t>(d_out[c++]));
        }
    }
}

Task::Task(const std::string &name, std::initializer_list<TagPort> ports, std::function<int(uint8_t*[], uint8_t*[])> codelet) : name(name), ports(ports.size()) {
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

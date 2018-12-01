//
// Created by tomas on 11/23/18.
//

#ifndef CMPXCHG_TASK_H
#define CMPXCHG_TASK_H


#include <cstdio>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Port.h"

class Module;
class Task;
struct TagPort {
    enum class tag_type_t : uint8_t {port_in, port_out};
    tag_type_t tag_type;
    const std::string &name;
    uint8_t id;
    const size_t size;
    TagPort(const tag_type_t tagType,  uint8_t id, const std::string &name, const size_t size) :
        tag_type(tagType), id(id), name(name), size(size) {}
};

struct TagPortIn : public TagPort {
    TagPortIn(const std::string &name,  uint8_t id) : TagPort(tag_type_t::port_in, id, name, 0) {}
};
struct TagPortOut : public TagPort {

    TagPortOut(const std::string &name,  uint8_t id, uint64_t size) : TagPort(tag_type_t::port_out, id, name, size) {}
};
class Task {
    friend Module;
    const std::string &name;
    std::vector<std::shared_ptr<Port>> ports;
    std::vector<std::shared_ptr<Port>> ports_in;
    std::vector<std::shared_ptr<Port>> ports_out;

    std::function<int(uint8_t*[], uint8_t*[])> codelet;

    void exec();

protected:
    Port& create_port(const std::string &name,  uint8_t id);
    Port& create_port_in(const std::string &name,  uint8_t id);
    Port& create_port_out(const std::string &name,  uint8_t id, size_t n_elems, uint64_t n_buf = 3);
    void create_codelet(std::function<int(uint8_t*[], uint8_t*[])> codelet);
public:
    Task(const std::string &name, std::initializer_list<TagPort> ports, std::function<int(uint8_t*[], uint8_t*[])> codelet);
    inline Port& operator[](const int id)
    {
        return *this->ports[id];
    }

    std::vector<std::shared_ptr<std::vector<uint8_t >>> buf;
};


#endif //CMPXCHG_TASK_H

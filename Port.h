//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_PORT_H
#define CMPXCHG_PORT_H

#include <memory>
#include <cstdint>
#include <vector>

class Port {
    const std::string &name;
    std::shared_ptr<uint64_t> shared_var;

    inline bool cas(volatile uint64_t *src,
                    uint64_t cmp,
                    uint64_t with)
    {
        return __sync_bool_compare_and_swap(src, cmp, with);
    }

    inline uint64_t tas(volatile uint64_t *src,
                        uint64_t cmp)
    {
        return __sync_lock_test_and_set(src, cmp);
    }



public:
    Port(const std::string &name) :
            name(name) {
    };
    void bind(Port& port) {
        shared_var = std::make_shared<uint64_t >(uint64_t {0});
        port.shared_var = shared_var;
    }

    uint64_t poll() { uint64_t v; while(!(v=tas(shared_var.get(), 0))); return v;}
    void put(uint64_t v) { while(!cas(shared_var.get(), 0, v)); }
};


#endif //CMPXCHG_PORT_H

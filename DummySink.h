//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_DUMMYSINK_H
#define CMPXCHG_DUMMYSINK_H

#include <cstdint>
#include <iostream>
#include "Module.h"

namespace module {
    namespace dummysink {
        enum class tsk : uint8_t {sink, SIZE};

        namespace port {
            enum class sink : uint8_t {
                p_in, SIZE
            };
        }
    }
}

class DummySink : public Module {

public:
    inline Task&   operator[](const module::dummysink::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::dummysink::port::sink p) { return Module::operator[]((int)module::dummysink::tsk::sink)[(int)p]; }


    DummySink() {
        Task & t_sink = create_task("sink", {
            TagPortIn("p_in", (uint8_t )module::dummysink::port::sink::p_in)
        },[this]() -> int {
                Port& p = this->operator[](module::dummysink::port::sink ::p_in);
                for(uint64_t i= 0; true ;i++) {
                    uint8_t *d_in = (uint8_t *)p.poll();
                }
        });
    }

};


#endif //CMPXCHG_DUMMYSINK_H

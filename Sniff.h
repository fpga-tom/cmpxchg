//
// Created by tomas on 12/1/18.
//

#ifndef CMPXCHG_SNIFF_H
#define CMPXCHG_SNIFF_H

#include <cstdint>
#include "Module.h"

namespace module {
    namespace sniff {
        enum class tsk : uint8_t {sniff, SIZE};

        namespace port {
            enum class sniff : uint8_t {
                p_out, SIZE
            };
        }
    }
}


class Sniff : public Module {
    inline Task&   operator[](const module::sniff::tsk           t) { return Module::operator[]((int)t);                          }
    Port& port;
    const int buf_size;
public:
    inline Port& operator[](const module::sniff::port::sniff p) { return Module::operator[]((int)module::sniff::tsk::sniff)[(int)p]; }
    Sniff( Port& port, const int buf_size) : port(port), buf_size(buf_size) {
        Task & t_sniff = create_task("sniff", {
                TagPortOut("p_out", (uint8_t )module::sniff::port::sniff ::p_out, buf_size)
        },[this, buf_size](uint8_t** d_in, uint8_t **d_out) -> int {
            uint64_t* b = reinterpret_cast<uint64_t *>(this->port.sniff());
            uint64_t *d = reinterpret_cast<uint64_t *>(d_out[0]);
            if(b != nullptr) {
                for(int i = 0; i < buf_size/sizeof(uint64_t); i++) {
                    d[i] = b[i];
                }
            }
            return 0;
        });
    }

};


#endif //CMPXCHG_SNIFF_H

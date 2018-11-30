//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_SYNC_H
#define CMPXCHG_SYNC_H

#include <cstdint>
#include <iostream>
#include <cmath>
#include "Module.h"

namespace module {
    namespace sync {
        enum class tsk : uint8_t {correlate, find_peak, lock, align, SIZE};

        namespace port {
            enum class correlate : uint8_t {p_in, p_out, SIZE};
            enum class find_peak : uint8_t {p_in, p_out, SIZE};
            enum class lock : uint8_t {p_in, p_out, SIZE};
            enum class align : uint8_t {p_in, p_out, SIZE};
        }
    }
}

class Sync : public Module {
public:
    inline Task&   operator[](const module::sync::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::sync::port::correlate p) { return Module::operator[]((int)module::sync::tsk::correlate)[(int)p]; }
    inline Port& operator[](const module::sync::port::find_peak p) { return Module::operator[]((int)module::sync::tsk::find_peak)[(int)p]; }
    inline Port& operator[](const module::sync::port::lock p) { return Module::operator[]((int)module::sync::tsk::lock)[(int)p]; }
    inline Port& operator[](const module::sync::port::align p) { return Module::operator[]((int)module::sync::tsk::align)[(int)p]; }


    const int buf_size;
    Sync(const int buf_size=10240) : buf_size(buf_size) {
        Task & t_correlate = create_task("correlate");


        t_correlate.create_port_in("p_in");
        Port & p_correlate = t_correlate.create_port_out("p_out", buf_size);

        t_correlate.create_codelet([this]() -> int {
            Port& p = this->operator[](module::sync::port::correlate ::p_in);
            Port& p_out = this->operator[](module::sync::port::correlate ::p_out);
            for(uint64_t i= 0; true ;i++) {
                uint8_t *d_in = (uint8_t *)p.poll();
                std::shared_ptr<std::vector<uint8_t >> &v_out = this->operator[](module::sync::tsk::correlate).buf[i%2];

                correlate(d_in, v_out->data());

                p_out.put(reinterpret_cast<uint64_t>(v_out->data()));
            }

            return 0;
        });


        Task & t_find_peak = create_task("find_peak");
        t_find_peak.create_port_in("p_in").bind(p_correlate);
        Port & p_find_peak = t_find_peak.create_port_out("p_out", buf_size);
        t_find_peak.create_codelet([this]() -> int {
            Port& p = this->operator[](module::sync::port::find_peak ::p_in);
            Port& p_out = this->operator[](module::sync::port::find_peak ::p_out);
            for(uint64_t i= 0; true ;i++) {
                uint8_t *d_in = (uint8_t *)p.poll();
                std::shared_ptr<std::vector<uint8_t >> &v_out = this->operator[](module::sync::tsk::find_peak).buf[i%2];

                find_peak(d_in, v_out->data());

                p_out.put(reinterpret_cast<uint64_t>(v_out->data()));
            }
            return 0;
        });


        Task & t_lock = create_task("lock");
        t_lock.create_port_in("p_in").bind(p_find_peak);
        Port & p_lock = t_lock.create_port_out("p_out", buf_size);
        t_lock.create_codelet([this]() -> int {
            Port& p = this->operator[](module::sync::port::lock ::p_in);
            Port& p_out = this->operator[](module::sync::port::lock ::p_out);
            for(uint64_t i= 0; true ;i++) {
                std::shared_ptr<std::vector<uint8_t >> &v_out = this->operator[](module::sync::tsk::lock).buf[i%2];

                uint8_t *d_in = (uint8_t *)p.poll();
                lock(d_in,v_out->data());

                p_out.put(reinterpret_cast<uint64_t>(v_out->data()));
            }

            return 0;
        });

        Task & t_align = create_task("align");
        t_align.create_port_in("p_in").bind(p_lock);
        t_align.create_port_out("p_out", buf_size);
        t_align.create_codelet([this]() -> int {
            Port& p = this->operator[](module::sync::port::align ::p_in);
            Port& p_out = this->operator[](module::sync::port::align ::p_out);
            for(uint64_t i= 0; true ;i++) {
                std::shared_ptr<std::vector<uint8_t >> &v_out = this->operator[](module::sync::tsk::align).buf[i%2];

                uint8_t *d_in = (uint8_t *)p.poll();
                align(d_in,v_out->data());

                p_out.put(reinterpret_cast<uint64_t>(v_out->data()));
            }

            return 0;
        });
    }


    void correlate(uint8_t *d_in, uint8_t *d_out) {
        for(int i = 0;i  < buf_size; i++) {
            d_out[i] += (d_in[i] + i);
        }

    }
    void find_peak(uint8_t *d_in, uint8_t *d_out) {
        for(int i = 0;i  < buf_size; i++) {
            d_out[i] += d_in[i] + i;
        }
    }
    void lock(uint8_t *d_in, uint8_t *d_out) {
        for(int i = 0;i  < buf_size; i++) {
            d_out[i] += d_in[i] + i;
        }
    }
    void align(uint8_t *d_in, uint8_t *d_out) {
        for(int i = 0;i  < buf_size; i++) {
            d_out[i] += d_in[i] + i;
        }
    }


};



#endif //CMPXCHG_SYNC_H

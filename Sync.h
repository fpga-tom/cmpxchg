//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_SYNC_H
#define CMPXCHG_SYNC_H

#include <cstdint>
#include <iostream>
#include <cmath>
#include <complex>
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
    std::complex<float> delay;
    Sync(const int buf_size=10240) : buf_size(buf_size), delay(buf_size+8192) {

        Task & t_correlate = create_task("correlate", {
            TagPortIn("p_in", (uint8_t )module::sync::port::correlate::p_in),
            TagPortOut("p_out", (uint8_t)module::sync::port::correlate::p_out, buf_size*sizeof(float))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->correlate(d_in, d_out);});


        Task & t_find_peak = create_task("find_peak", {
                TagPortIn("p_in", (uint8_t)module::sync::port::find_peak ::p_in),
                TagPortOut("p_out", (uint8_t)module::sync::port::find_peak ::p_out, buf_size*sizeof(float))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->find_peak(d_in, d_out);});


        this->operator[](module::sync::port::find_peak ::p_in).bind(
                this->operator[](module::sync::port::correlate ::p_out));

        Task & t_lock = create_task("lock", {
            TagPortIn("p_in", (uint8_t)module::sync::port::lock ::p_in),
            TagPortOut("p_out", (uint8_t)module::sync::port::lock ::p_out, buf_size*sizeof(float))
        }, [this](uint8_t** d_in, uint8_t **d_out) -> int {return this->lock(d_in, d_out);});

        this->operator[](module::sync::port::lock ::p_in).bind(
                this->operator[](module::sync::port::find_peak ::p_out));


        Task & t_align = create_task("align", {
                TagPortIn("p_in", (uint8_t)module::sync::port::align ::p_in),
                TagPortOut("p_out", (uint8_t)module::sync::port::align ::p_out, buf_size*sizeof(float))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->align(d_in, d_out);});

        this->operator[](module::sync::port::align ::p_in).bind(
                this->operator[](module::sync::port::lock ::p_out));
    }

    int correlate(uint8_t **d_in, uint8_t **d_out);
    int find_peak(uint8_t **d_in, uint8_t **d_out);
    int lock(uint8_t **d_in, uint8_t **d_out);
    int align(uint8_t **d_in, uint8_t **d_out);
};



#endif //CMPXCHG_SYNC_H

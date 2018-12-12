//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_SYNC_H
#define CMPXCHG_SYNC_H

#include <cstdint>
#include <iostream>
#include <cmath>
#include <complex>
#include <deque>
#include <liquid/liquid.h>
#include "Module.h"

namespace module {
    namespace sync {
        enum class tsk : uint8_t {resample, correlate, find_peak, lock, align, SIZE};

        namespace port {
            enum class resample : uint8_t {p_in, p_out, SIZE};
            enum class correlate : uint8_t {p_in, p_out, SIZE};
            enum class find_peak : uint8_t {p_in, p_out, SIZE};
            enum class lock : uint8_t {p_in, p_out, SIZE};
            enum class align : uint8_t {p_in, p_out, SIZE};
        }
    }
}

class Sync : public Module {
    inline Task&   operator[](const module::sync::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::sync::port::resample p) { return Module::operator[]((int)module::sync::tsk::resample)[(int)p]; }
    inline Port& operator[](const module::sync::port::correlate p) { return Module::operator[]((int)module::sync::tsk::correlate)[(int)p]; }
    inline Port& operator[](const module::sync::port::find_peak p) { return Module::operator[]((int)module::sync::tsk::find_peak)[(int)p]; }
    inline Port& operator[](const module::sync::port::lock p) { return Module::operator[]((int)module::sync::tsk::lock)[(int)p]; }
    inline Port& operator[](const module::sync::port::align p) { return Module::operator[]((int)module::sync::tsk::align)[(int)p]; }
public:
    Port& p_in() { return this->operator[](module::sync::port::resample::p_in); }
    Port& p_corr_out() { return this->operator[](module::sync::port::correlate::p_out); }
    Port& p_out() { return this->operator[](module::sync::port::align ::p_out); }



    const int buf_size;
    const uint64_t fft_len = 8192;
    const uint64_t sym_len = 10240;
    const uint64_t cp_len = 2048;
    float acc;
    std::vector<std::complex<float>> delay;
    std::deque<float> acc_delay;
    std::vector<float> b;


    resamp_crcf q;
    const uint P = 64;
    const uint Q = 70;
    const uint m = 12;
    const float bw = 0.45f;
    const float As = 60;
    const float f_a=1,f_b=1,f_c=1,f_d=1,f_g=1,f_h=1,f_i=1,f_j=1,f_k=1, f_e=1, f_f=1;

    Sync(const int buf_size=10240*70/64) : buf_size(buf_size), delay(sym_len+fft_len), b(sym_len), acc(0.f), acc_delay(cp_len)  {

        Task & t_resamp = create_task("resamp", {
                TagPortIn("p_in", (uint8_t )module::sync::port::resample ::p_in),
                TagPortOut("p_out", (uint8_t)module::sync::port::resample ::p_out, sym_len*sizeof(std::complex<float>))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->resample(d_in, d_out);});

        Task & t_correlate = create_task("correlate", {
            TagPortIn("p_in", (uint8_t )module::sync::port::correlate::p_in),
            TagPortOut("p_out", (uint8_t)module::sync::port::correlate::p_out, sym_len*sizeof(std::complex<float>))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->correlate(d_in, d_out);});

        this->operator[](module::sync::port::correlate ::p_in).bind(
                this->operator[](module::sync::port::resample ::p_out));


        Task & t_find_peak = create_task("find_peak", {
                TagPortIn("p_in", (uint8_t)module::sync::port::find_peak ::p_in),
                TagPortOut("p_out", (uint8_t)module::sync::port::find_peak ::p_out, sym_len*sizeof(std::complex<float>))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->find_peak(d_in, d_out);});


        this->operator[](module::sync::port::find_peak ::p_in).bind(
                this->operator[](module::sync::port::correlate ::p_out));

        Task & t_lock = create_task("lock", {
            TagPortIn("p_in", (uint8_t)module::sync::port::lock ::p_in),
            TagPortOut("p_out", (uint8_t)module::sync::port::lock ::p_out, sym_len*sizeof(std::complex<float>))
        }, [this](uint8_t** d_in, uint8_t **d_out) -> int {return this->lock(d_in, d_out);});

        this->operator[](module::sync::port::lock ::p_in).bind(
                this->operator[](module::sync::port::find_peak ::p_out));


        Task & t_align = create_task("align", {
                TagPortIn("p_in", (uint8_t)module::sync::port::align ::p_in),
                TagPortOut("p_out", (uint8_t)module::sync::port::align ::p_out, std::ceil(sym_len*sizeof(std::complex<float>)*P/(float)Q))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->align(d_in, d_out);});

        this->operator[](module::sync::port::align ::p_in).bind(
                this->operator[](module::sync::port::lock ::p_out));
    }

    int resample(uint8_t **d_in, uint8_t **d_out);
    int correlate(uint8_t **d_in, uint8_t **d_out);
    int find_peak(uint8_t **d_in, uint8_t **d_out);
    int lock(uint8_t **d_in, uint8_t **d_out);
    int align(uint8_t **d_in, uint8_t **d_out);
};



#endif //CMPXCHG_SYNC_H

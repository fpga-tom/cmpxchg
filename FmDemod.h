//
// Created by tomas on 12/1/18.
//

#ifndef CMPXCHG_FMDEMOD_H
#define CMPXCHG_FMDEMOD_H

#include <cstdint>
#include <complex>
#include <liquid/liquid.h>
#include "Module.h"

namespace module {
    namespace fm_demod {
        enum class tsk : uint8_t {downsample_first, demod, downsample_second, SIZE};

        namespace port {
            enum class downsample_first : uint8_t {p_in, p_out, SIZE};
            enum class demod : uint8_t {p_in, p_out, SIZE};
            enum class downsample_second : uint8_t {p_in, p_out, SIZE};
        }
    }
}

class FmDemod : public Module {
    inline Task&   operator[](const module::fm_demod::tsk           t) { return Module::operator[]((int)t);                          }

    inline Port& operator[](const module::fm_demod::port::demod p) { return Module::operator[]((int)module::fm_demod::tsk::demod)[(int)p]; }


    const int buf_size;
    int down_one;
    int down_two;

    firdecim_crcf q_one;
    firdecim_rrrf q_two;

    freqdem q_demod;
public:
    inline Port& operator[](const module::fm_demod::port::downsample_first p) { return Module::operator[]((int)module::fm_demod::tsk::downsample_first)[(int)p]; }
    inline Port& operator[](const module::fm_demod::port::downsample_second p) { return Module::operator[]((int)module::fm_demod::tsk::downsample_second)[(int)p]; }
    FmDemod(const int buf_size, int down_one, int down_two) :
        buf_size(buf_size), down_one(down_one), down_two(down_two) {

        q_one = firdecim_crcf_create_kaiser(down_one,8,30.);
        q_two = firdecim_rrrf_create_kaiser(down_two,8,60.);

        q_demod = freqdem_create(.75);


        Task & t_downsample_first = create_task("first", {
                TagPortIn("p_in", (uint8_t )module::fm_demod::port::downsample_first::p_in),
                TagPortOut("p_out", (uint8_t)module::fm_demod::port::downsample_first::p_out, buf_size/down_one*sizeof(std::complex<float>))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->downsample_first(d_in, d_out);});

        Task & t_demod = create_task("demod", {
                TagPortIn("p_in", (uint8_t )module::fm_demod::port::demod::p_in),
                TagPortOut("p_out", (uint8_t)module::fm_demod::port::demod::p_out, buf_size/down_one*sizeof(float))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->demod(d_in, d_out);});

        Task & t_downsample_second = create_task("second", {
                TagPortIn("p_in", (uint8_t )module::fm_demod::port::downsample_second::p_in),
                TagPortOut("p_out", (uint8_t)module::fm_demod::port::downsample_second::p_out, (buf_size/down_one/down_two)*sizeof(float))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->downsample_second(d_in, d_out);});

        this->operator[](module::fm_demod::port::demod ::p_in).bind(
                this->operator[](module::fm_demod::port::downsample_first ::p_out));

        this->operator[](module::fm_demod::port::downsample_second ::p_in).bind(
                this->operator[](module::fm_demod::port::demod ::p_out));

    }

    int downsample_first(uint8_t**, uint8_t**);
    int demod(uint8_t**, uint8_t**);
    int downsample_second(uint8_t**, uint8_t**);

};


#endif //CMPXCHG_FMDEMOD_H


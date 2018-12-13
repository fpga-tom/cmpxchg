//
// Created by tomas on 12/13/18.
//

#ifndef CMPXCHG_GSMDEMOD_H
#define CMPXCHG_GSMDEMOD_H

#include <cstdint>
#include <complex>
#include <liquid/liquid.h>
#include "Module.h"

namespace module {
    namespace gsm_demod {
        enum class tsk : uint8_t {downsample, sync, SIZE};

        namespace port {
            enum class downsample : uint8_t {p_in, p_out, SIZE};
            enum class sync : uint8_t {p_in, p_out, SIZE};
        }
    }
}

class GsmDemod : public Module {
    inline Task&   operator[](const module::gsm_demod::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::gsm_demod::port::sync p) { return Module::operator[]((int)module::gsm_demod::tsk::sync)[(int)p]; }
    inline Port& operator[](const module::gsm_demod::port::downsample p) { return Module::operator[]((int)module::gsm_demod::tsk::downsample)[(int)p]; }

    const uint32_t buf_size;
    firdecim_crcf q_one;
    nco_crcf q_nco;
    uint32_t down_one;
    float proportional, integral;
    const float P_part = 1e-4;
    const float I_part = 1e-6;

    int downsample(uint8_t **d_in, uint8_t **d_out);
    int sync(uint8_t **d_in, uint8_t **d_out);

public:
    Port& p_in() { return this->operator[](module::gsm_demod::port::downsample ::p_in); }
    Port& p_out() { return this->operator[](module::gsm_demod::port::sync::p_out); }

    GsmDemod(uint32_t buf_size, uint32_t down_one) : buf_size(buf_size), down_one(down_one),
        proportional(0.f), integral(.0f) {
        q_one = firdecim_crcf_create_kaiser(down_one,8,20.);

        q_nco = nco_crcf_create(LIQUID_NCO);
        nco_crcf_set_phase(q_nco, 0.0f);
        nco_crcf_set_frequency(q_nco, 0.0f);

        Task & t_downsample = create_task("down", {
                TagPortIn("p_in", (uint8_t )module::gsm_demod::port::downsample::p_in),
                TagPortOut("p_out", (uint8_t)module::gsm_demod::port::downsample::p_out, buf_size/down_one*sizeof(std::complex<float>))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->downsample(d_in, d_out);});

        Task & t_sync = create_task("sync", {
                TagPortIn("p_in", (uint8_t )module::gsm_demod::port::sync::p_in),
                TagPortOut("p_out", (uint8_t)module::gsm_demod::port::sync::p_out, buf_size/down_one*sizeof(std::complex<float>))
        },[this](uint8_t** d_in, uint8_t **d_out) -> int {return this->sync(d_in, d_out);});

        this->operator[](module::gsm_demod::port::sync ::p_in).bind(
                this->operator[](module::gsm_demod::port::downsample ::p_out));
    }
};


#endif //CMPXCHG_GSMDEMOD_H

//
// Created by tomas on 12/12/18.
//

#ifndef CMPXCHG_NCO_H
#define CMPXCHG_NCO_H

#include <cstdint>
#include <complex>
#include "Module.h"
#include <liquid/liquid.h>

namespace module {
    namespace nco {
        enum class tsk : uint8_t {forward, SIZE};

        namespace port {
            enum class forward : uint8_t {
                p_in, p_out, SIZE
            };
        }
    }
}


class NCO : public Module {
    inline Task&   operator[](const module::nco::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::nco::port::forward p) { return Module::operator[]((int)module::nco::tsk::forward)[(int)p]; }
    const int buf_size;
    nco_crcf q;

public:

    Port& p_in() { return this->operator[](module::nco::port::forward::p_in);}
    Port& p_out() { return this->operator[](module::nco::port::forward::p_out);}

    void tune(float freq) { nco_crcf_set_frequency(q, freq*2*M_PI); }
    NCO(const int buf_size) : buf_size(buf_size) {

        q = nco_crcf_create(LIQUID_NCO);
        nco_crcf_set_phase(q, 0.0f);
        nco_crcf_set_frequency(q, 0.0f);

        Task & t_forward = create_task("nco", {
                TagPortIn("p_in", (uint8_t )module::nco::port::forward ::p_in),
                TagPortOut("p_out", (uint8_t )module::nco::port::forward ::p_out, buf_size*sizeof(std::complex<float>))
        },[this, buf_size](uint8_t** d_in, uint8_t **d_out) -> int {
            std::complex<float> *d = reinterpret_cast<std::complex<float> *>(d_in[0]);
            std::complex<float> *d_o = reinterpret_cast<std::complex<float> *>(d_out[0]);
            nco_crcf_mix_block_down(q, d, d_o, buf_size);
            return 0;
        });

    }

};


#endif //CMPXCHG_NCO_H

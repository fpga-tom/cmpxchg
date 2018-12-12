//
// Created by tomas on 12/1/18.
//

#ifndef CMPXCHG_SPECTROGRAM_H
#define CMPXCHG_SPECTROGRAM_H

#include <cstdint>
#include <complex>
#include "Module.h"

namespace module {
    namespace spectrogram {
        enum class tsk : uint8_t {spectrum, SIZE};

        namespace port {
            enum class spectrum : uint8_t {
                p_in, p_out, SIZE
            };
        }
    }
}


class Spectrogram : public Module {
    inline Task&   operator[](const module::spectrogram::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::spectrogram::port::spectrum p) { return Module::operator[]((int)module::spectrogram::tsk::spectrum)[(int)p]; }
public:
    Port& p_in() { return this->operator[](module::spectrogram::port::spectrum::p_in);}
    Port& p_out() {return this->operator[](module::spectrogram::port::spectrum::p_out);}
    const int buf_size;
    Spectrogram(const int buf_size) : buf_size(buf_size) {

        Task & t_spectrum = create_task("spectrum", {
                TagPortIn("p_in", (uint8_t )module::spectrogram::port::spectrum ::p_in),
                TagPortOut("p_out", (uint8_t )module::spectrogram::port::spectrum ::p_out, buf_size*sizeof(float))
        },[this, buf_size](uint8_t** d_in, uint8_t **d_out) -> int {
            std::complex<float> *d = reinterpret_cast<std::complex<float> *>(d_in[0]);
            float *d_o = reinterpret_cast<float *>(d_out[0]);
            for(int i = 0;i < buf_size; i++) {
                d_o[i] = log10(std::norm(d[i]))*10;
            }
            return 0;
        });

    }

};


#endif //CMPXCHG_SPECTROGRAM_H

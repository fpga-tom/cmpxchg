//
// Created by tomas on 12/1/18.
//

#ifndef CMPXCHG_FFT_H
#define CMPXCHG_FFT_H

#include <cstdint>
#include <fftw3.h>
#include <cstring>
#include <liquid/liquid.h>
#include <complex>
#include "Module.h"

namespace module {
    namespace fft {
        enum class tsk : uint8_t {forward, SIZE};

        namespace port {
            enum class forward : uint8_t {
                p_in, p_out, SIZE
            };
        }
    }
}


class FFT : public Module {
    inline Task&   operator[](const module::fft::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::fft::port::forward p) { return Module::operator[]((int)module::fft::tsk::forward)[(int)p]; }
    const int buf_size;
    fftwf_complex *inBuf, *outBuf;
    fftwf_plan_s *plan;
    std::vector<float> win;

public:

    Port& p_in() { return this->operator[](module::fft::port::forward::p_in);}
    Port& p_out() { return this->operator[](module::fft::port::forward::p_out);}
    FFT(const int buf_size) : buf_size(buf_size), win(buf_size) {

        inBuf = reinterpret_cast<fftwf_complex*>(fftwf_malloc(
                sizeof(fftwf_complex) * buf_size));

        outBuf = reinterpret_cast<fftwf_complex*>(fftwf_malloc(
                sizeof(fftwf_complex) * buf_size));

        plan = fftwf_plan_dft_1d(buf_size,
                                        inBuf, outBuf, FFTW_FORWARD, FFTW_ESTIMATE);

        for(int i = 0; i < buf_size; i++) {
            win[i] = hann(i, buf_size);
        }

        Task & t_forward = create_task("forward", {
                TagPortIn("p_in", (uint8_t )module::fft::port::forward ::p_in),
                TagPortOut("p_out", (uint8_t )module::fft::port::forward ::p_out, buf_size*sizeof(fftwf_complex))
        },[this, buf_size](uint8_t** d_in, uint8_t **d_out) -> int {
            std::complex<float> *d = reinterpret_cast<std::complex<float> *>(d_in[0]);
            for(int i = 0; i < buf_size; i++) {
                d[i] *= win[i];
            }
            memcpy(inBuf, d, sizeof(fftwf_complex) * buf_size);
            fftwf_execute(plan);
            memcpy(d_out[0], &outBuf[buf_size/2], sizeof(fftwf_complex) * buf_size/2);
            memcpy(&d_out[0][buf_size*sizeof(fftwf_complex )/2], outBuf, sizeof(fftwf_complex) * buf_size/2);
            return 0;
        });

    }

};


#endif //CMPXCHG_FFT_H

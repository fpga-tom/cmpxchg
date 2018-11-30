//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_SHORTTOCOMPLEX_H
#define CMPXCHG_SHORTTOCOMPLEX_H

#include <cstdint>
#include <complex>
#include "Module.h"

namespace module {
    namespace short_to_complex {
        enum class tsk : uint8_t {convert, SIZE};

        namespace port {
            enum class convert : uint8_t {
                p_in_r, p_in_i, p_out, SIZE
            };
        }
    }
}


class ShortToComplex : public Module {
public:
    inline Task&   operator[](const module::short_to_complex::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::short_to_complex::port::convert p) { return Module::operator[]((int)module::short_to_complex::tsk::convert)[(int)p]; }

    const int buf_size;
    ShortToComplex(const int buf_size) : buf_size(buf_size) {
        Task & t_convert = create_task("convert");
        Port & p_in_r = t_convert.create_port_in("p_in_r");
        Port & p_in_i = t_convert.create_port_in("p_in_i");

        Port & p_out = t_convert.create_port_out("p_out", buf_size*sizeof(std::complex<float>));

        t_convert.create_codelet([this, &t_convert, &p_in_r, &p_in_i, &p_out]() -> int {
            for(uint64_t i= 0; true ;i++) {
                uint16_t *d_in_r = (uint16_t *)p_in_r.poll();
                uint16_t *d_in_i = (uint16_t *)p_in_i.poll();

                std::complex<float> * buf = reinterpret_cast<std::complex<float> *>(t_convert.buf[i % 2]->data());
                convert(d_in_r, d_in_i, buf);

                p_out.put(reinterpret_cast<uint64_t>(buf));
            }

            return 0;
        });
    }

    void convert(uint16_t *real, uint16_t *imag, std::complex<float> *d_out) {
        for(int i = 0; i < buf_size; i++) {
            d_out[i] = std::complex<float>{(float)real[i], (float)imag[i]};
        }
    }
};


#endif //CMPXCHG_SHORTTOCOMPLEX_H

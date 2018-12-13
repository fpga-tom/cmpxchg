//
// Created by tomas on 12/13/18.
//

#include "GsmDemod.h"

int GsmDemod::downsample(uint8_t **d_in, uint8_t **d_out) {
    std::complex<float> *d = reinterpret_cast<std::complex<float> *>(d_in[0]);
    std::complex<float> *o = reinterpret_cast<std::complex<float> *>(d_out[0]);
    firdecim_crcf_execute_block(q_one, d, buf_size/down_one, o);
    return 0;
}

int GsmDemod::sync(uint8_t **d_in, uint8_t **d_out) {
    std::complex<float> *d = reinterpret_cast<std::complex<float> *>(d_in[0]);
    std::complex<float> *o = reinterpret_cast<std::complex<float> *>(d_out[0]);
    for(int i = 0; i < buf_size/down_one; i++) {
        std::complex<float> y;
        nco_crcf_cexpf(q_nco, &y);
        std::complex<float> diff = d[i]*std::conj(y);
        float err = diff.real() * diff.imag();
        integral += err*P_part;
        proportional = err*I_part;
        nco_crcf_adjust_phase(q_nco, integral + proportional);
        nco_crcf_step(q_nco);
        o[i] = diff;
    }
    return 0;
}

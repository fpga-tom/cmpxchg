//
// Created by tomas on 12/1/18.
//

#include "FmDemod.h"

int FmDemod::downsample_first(uint8_t **d_in, uint8_t **d_out) {
    std::complex<float> *d = reinterpret_cast<std::complex<float> *>(d_in[0]);
    std::complex<float> *o = reinterpret_cast<std::complex<float> *>(d_out[0]);
    firdecim_crcf_execute_block(q_one, d, buf_size/down_one, o);
    return 0;
}

int FmDemod::demod(uint8_t **d_in, uint8_t **d_out) {
    std::complex<float> *d = reinterpret_cast<std::complex<float> *>(d_in[0]);
    float *o = reinterpret_cast<float *>(d_out[0]);
    freqdem_demodulate_block(q_demod, d, buf_size/down_one, o);
    return 0;
}

int FmDemod::downsample_second(uint8_t **d_in, uint8_t **d_out) {
    float *d = reinterpret_cast<float *>(d_in[0]);
    float *o = reinterpret_cast<float *>(d_out[0]);
    firdecim_rrrf_execute_block(q_two, d, buf_size/down_one/down_two, o);
    return 0;
}

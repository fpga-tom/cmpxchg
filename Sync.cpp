//
// Created by tomas on 11/30/18.
//

#include <algorithm>
#include "Sync.h"

int Sync::correlate(uint8_t **d_in, uint8_t **d_out) {
    std::complex<float> * in = reinterpret_cast<std::complex<float> *>(d_in[0]);
    std::copy(in, in + buf_size, delay.begin() + fft_len);

    for(int i = 0;i < sym_len; i++) {
        b[i] = std::abs(in[i] * std::conj(delay[i]));
    }


    std::copy(begin(delay) + sym_len, end(delay), begin(delay));

    std::transform(begin(b), end(b),  begin(b),
                   [&](auto prod) {
                       acc = acc + prod;
                       acc = acc - acc_delay.front();
                       acc_delay.pop_front();
                       acc_delay.push_back(prod);

                       return acc;
                   });
    std::copy(begin(b), end(b), d_out[0]);
    return 0;

}
int Sync::find_peak(uint8_t **d_in, uint8_t **d_out) {
    for(int i = 0;i  < buf_size; i++) {
        d_out[0][i] = d_in[0][i];
    }
    return 0;
}
int Sync::lock(uint8_t **d_in, uint8_t **d_out) {
    for(int i = 0;i  < buf_size; i++) {
        d_out[0][i] = d_in[0][i];
    }
    return 0;
}
int Sync::align(uint8_t **d_in, uint8_t **d_out) {
    for(int i = 0;i  < buf_size; i++) {
        d_out[0][i] = d_in[0][i];
    }
    return 0;
}

int Sync::resample(uint8_t **d_in, uint8_t **d_out) {
    std::complex<float> *d = reinterpret_cast<std::complex<float> *>(d_in[0]);
    std::complex<float> *d_o = reinterpret_cast<std::complex<float> *>(d_out[0]);
/*
    std::complex<float> y[8];
    for(int i = 0; i < buf_size-5; i+=5) {
        for(int j = 0; j < 10; j+=5) {
            y[0] = f_k * d[i+j] + f_g * d[i + 1] + f_c * d[i + 2];
            y[1] = f_h * d[i + 2 +j] + f_d * d[i + 3];
            y[2] = f_i * d[i + 3 +j] + f_e * d[i + 4] + f_a * d[i + 5];
            y[3] = f_j * d[i + 4+j] + f_f * d[i + 5] + f_b * d[i + 6];
        }



    }
    std::complex<float> x[8];
    */
    return 0;
}

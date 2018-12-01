//
// Created by tomas on 12/1/18.
//

#include <iostream>
#include "PaAudio.h"



int PaAudio::play(uint8_t **d_in, uint8_t **d_out) {
    int16_t buf[buf_size];
    float *d = reinterpret_cast<float *>(d_in[0]);
    for(int i = 0;i < buf_size; i++) {
        buf[i] = d[i]*1.5e4;
    }
    if (pa_simple_write(s, buf, (size_t) buf_size, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
    }

    return 0;
}

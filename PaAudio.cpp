//
// Created by tomas on 12/1/18.
//

#include <iostream>
#include "PaAudio.h"



int PaAudio::play(uint8_t **d_in, uint8_t **d_out) {
    int16_t buf[buf_size];
    static int count = 0;
    static struct timespec clock_start;
    struct timespec clock;
    clock_gettime(CLOCK_MONOTONIC_RAW, &clock);
    if(count == 0) {
        clock_start = clock;
    }
    float *d = reinterpret_cast<float *>(d_in[0]);
    for(int i = 0;i < buf_size; i++) {
        buf[i] = d[i]*4e3;
    }
    if (pa_simple_write(s, buf, (size_t) buf_size, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
    }
    count ++;
    if(count > 10) {
        uint64_t delta_us = (clock.tv_sec - clock_start.tv_sec) * 1000000 + (clock.tv_nsec - clock_start.tv_nsec) / 1000;
//        std::cout << count << "/" << delta_us << "/" << 1000000*count/delta_us << std::endl;
    }

    return 0;
}

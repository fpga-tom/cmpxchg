//
// Created by tomas on 11/30/18.
//

#include "Sync.h"

int Sync::correlate(uint8_t **d_in, uint8_t **d_out) {
    for(int i = 0;i  < buf_size; i++) {
        d_out[0][i] += (d_in[0][i] + i);
    }
    return 0;

}
int Sync::find_peak(uint8_t **d_in, uint8_t **d_out) {
    for(int i = 0;i  < buf_size; i++) {
        d_out[0][i] += d_in[0][i] + i;
    }
    return 0;
}
int Sync::lock(uint8_t **d_in, uint8_t **d_out) {
    for(int i = 0;i  < buf_size; i++) {
        d_out[0][i] += d_in[0][i] + i;
    }
    return 0;
}
int Sync::align(uint8_t **d_in, uint8_t **d_out) {
    for(int i = 0;i  < buf_size; i++) {
        d_out[0][i] += d_in[0][i] + i;
    }
    return 0;
}
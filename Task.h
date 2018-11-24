//
// Created by tomas on 11/23/18.
//

#ifndef CMPXCHG_MODULE_H
#define CMPXCHG_MODULE_H


#include "Signal.h"

class Task {
    uint64_t K;
    uint64_t N;
public:
    Task(uint64_t K, uint64_t N) : K(K), N(N) {}

};


#endif //CMPXCHG_MODULE_H

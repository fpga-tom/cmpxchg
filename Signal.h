//
// Created by tomas on 11/23/18.
//

#ifndef CMPXCHG_BUFFER_H
#define CMPXCHG_BUFFER_H


#include <glob.h>
#include <cstdint>
#include <vector>

class Signal {
    uint64_t shared_var = 0;


    inline bool cas(volatile uint64_t *src,
                    uint64_t cmp,
                    uint64_t with)
    {
        return __sync_bool_compare_and_swap(src, cmp, with);
    }

    inline uint64_t tas(volatile uint64_t *src,
                        uint64_t cmp)
    {
        return __sync_lock_test_and_set(src, cmp);
    }
public:

    Signal() {};
    void get() { while(!tas(&shared_var, 0)); }
    void put() { while(!cas(&shared_var, 0, 1)); }




};


#endif //CMPXCHG_BUFFER_H

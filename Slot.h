//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_SLOT_H
#define CMPXCHG_SLOT_H


#include <cstdint>

class Slot {

    uint64_t *shared_var = 0;

    inline uint64_t tas(volatile uint64_t *src,
                        uint64_t cmp)
    {
        return __sync_lock_test_and_set(src, cmp);
    }

public:
    Slot() {};
    void bind(uint64_t* _shared_var) { shared_var = _shared_var; }
    void poll() { while(!tas(shared_var, 0)); }

};


#endif //CMPXCHG_SLOT_H

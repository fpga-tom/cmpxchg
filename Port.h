//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_PORT_H
#define CMPXCHG_PORT_H

#include <memory>
#include <cstdint>
#include <vector>
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>

class Port {
    const std::string &name;
    std::shared_ptr<uint64_t> shared_var;
    std::shared_ptr<uint64_t> shared_var_last;
//    std::shared_ptr<std::mutex> iio_mutex;
//    std::shared_ptr<std::condition_variable> iio_cond;

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
    Port(const std::string& name) :
            name(name) {
    };
    void bind(Port& port) {
        shared_var = std::make_shared<uint64_t >(uint64_t {0});
        shared_var_last = std::make_shared<uint64_t >(uint64_t {0});
//        iio_mutex = std::make_shared<std::mutex>();
//        iio_cond = std::make_shared<std::condition_variable>();
        port.shared_var = shared_var;
        port.shared_var_last = shared_var_last;
//        port.iio_mutex = iio_mutex;
//        port.iio_cond = iio_cond;
    }

    uint64_t poll() {
        uint64_t v;
//        std::unique_lock<std::mutex> lock(*iio_mutex);
        while(!(v=tas(shared_var.get(), 0)))
            std::this_thread::sleep_for(std::chrono::microseconds(300));
//            iio_cond->wait(lock);
        *shared_var_last = v;
        return v;
    }
    void put(uint64_t v) {
        while(!cas(shared_var.get(), 0, v))
            std::this_thread::sleep_for(std::chrono::microseconds(300));
//            iio_cond->notify_all();
    }

    uint64_t sniff() const {
        return *shared_var_last;
    }
};


#endif //CMPXCHG_PORT_H

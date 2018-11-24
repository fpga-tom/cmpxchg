#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include "Signal.h"

namespace types
{
    struct uint128_t
    {
        uint64_t lo;
        uint64_t hi;
    }
        __attribute__ (( __aligned__( 16 ) ));
}

//template< class T > inline bool cas( volatile T * src, T cmp, T with );
//
//template<> inline bool cas( volatile types::uint128_t * src, types::uint128_t cmp, types::uint128_t with )
//{
//    bool result;
//    __asm__ __volatile__
//    (
//    "lock cmpxchg16b oword ptr %1\n\t"
//    "setz %0"
//    : "=q" ( result )
//    , "+m" ( *src )
//    , "+d" ( cmp.hi )
//    , "+a" ( cmp.lo )
//    : "c" ( with.hi )
//    , "b" ( with.lo )
//    : "cc"
//    );
//    return result;
//}

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

uint64_t rdtsc(void)
{
    uint64_t var;
    uint32_t hi, lo;

    __asm volatile
    ("rdtsc" : "=a" (lo), "=d" (hi));

    var = ((uint64_t)hi << 32) | lo;
    return (var);
}

volatile uint64_t shared_var = 0;
volatile uint64_t timestamp = 0;


uint64_t fs = 10000000;
double T = 1e6/fs;
Signal sig;
std::vector<uint64_t > buf(fs);
void thread_1() {
//    uint64_t start = rdtsc();
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    int64_t op = 0;

    for(uint64_t i = 0; i < fs; i++) {
        buf[i] = i;
//        uint64_t op_start = rdtsc();
//        while(!cas(&shared_var, 0, i+1));
//        uint64_t op_end = rdtsc();
//        uint64_t d = op_end - op_start;
//        op += d;
    }
    sig.put();
//    while(!cas(&shared_var, 0, 1));
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
//    uint64_t end = rdtsc();
    std::cout << "done: " << delta_us/1000 << std::endl;
    std::cout << op/fs << std::endl;
}

void thread_2(int tid) {
    uint64_t local_var = 0;
    std::ofstream of{"/tmp/file.txt", std::ios::binary};
//    std::vector<uint64_t> *buf;
    struct timespec start, end;
    uint64_t last_timestamp = 0;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
//    while(!tas(&shared_var, 0));
    sig.get();
//    buf = (std::vector<uint64_t > *) local_var;
//    for(int i = 0; i < fs-1; i++) {
//        while (cas(&timestamp, last_timestamp, 0));
//        last_timestamp = timestamp;
//        local_var=tas(&shared_var, 0);

//        __sync_lock_release(&shared_var);

//        buf.emplace_back(local_var);

//    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    std::cout << "writing: " << delta_us/1000 << std::endl;
    for(int i = 0; i < fs; i++)
        of << buf[i] << std::endl;
}

uint64_t test = { 0xdecafbadfeedbeef };

int main()
{
    auto t1 = std::thread(thread_1);
    auto t2 = std::thread(thread_2, 0);
//    auto t3 = std::thread(thread_2, 1);
//    auto t4 = std::thread(thread_2, 2);
//    auto t5 = std::thread(thread_2, 3);
//    auto t6 = std::thread(thread_2, 4);
//    auto t7 = std::thread(thread_2, 5);
    t1.join();
    t2.join();
//    t3.join();
//    t4.join();
//    t5.join();
//    t6.join();
//    t7.join();
    return 0;
}
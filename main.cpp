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


uint64_t fs = 100000;
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

const uint64_t reg_a_len = 19;
const uint64_t reg_b_len = 22;
const uint64_t reg_c_len = 23;

const uint64_t reg_a_lsb = 0;
const uint64_t reg_b_lsb = reg_a_lsb + reg_a_len;
const uint64_t reg_c_lsb = reg_b_lsb + reg_b_len;

const uint64_t reg_a_msb = reg_a_lsb + reg_a_len - 1;
const uint64_t reg_b_msb = reg_b_lsb + reg_b_len - 1;
const uint64_t reg_c_msb = reg_c_lsb + reg_c_len - 1;

const uint64_t reg_a_mask = ((1UL << reg_a_len) - 1) << reg_a_lsb;
const uint64_t reg_b_mask = ((1UL << reg_b_len) - 1) << reg_b_lsb;
const uint64_t reg_c_mask = ((1UL << reg_c_len) - 1) << reg_c_lsb;

const uint64_t clk_bit_a = 1UL << (8 + reg_a_lsb);
const uint64_t clk_bit_b = 1UL << (10 + reg_b_lsb);
const uint64_t clk_bit_c = 1UL << (10 + reg_c_lsb);

const uint64_t data_bit_a[] = {1UL << (18 + reg_a_lsb),
                               1UL << (17 + reg_a_lsb),
                               1UL << (16 + reg_a_lsb),
                               1UL << (13 + reg_a_lsb)};

const uint64_t data_bit_b[] = {1UL << (21 + reg_b_lsb),
                               1UL << (20 + reg_b_lsb)};

const uint64_t data_bit_c[] = {1UL << (22 + reg_c_lsb),
                               1UL << (21 + reg_c_lsb),
                               1UL << (20 + reg_c_lsb),
                               1UL << (7 + reg_c_lsb)};

uint64_t reg_2(uint64_t i_state, int rounds = 64) {
    uint64_t r_state = 0;

    for (int i = 0; i < rounds; i++) {
        bool m_bit = ((i_state & clk_bit_a) != 0 && (i_state & clk_bit_b) != 0) ||
                     ((i_state & clk_bit_b) != 0 && (i_state & clk_bit_c) != 0) ||
                     ((i_state & clk_bit_c) != 0 && (i_state & clk_bit_a) != 0);
        bool clk_a = ((i_state & clk_bit_a) != 0) == m_bit;
        bool clk_b = ((i_state & clk_bit_b) != 0) == m_bit;
        bool clk_c = ((i_state & clk_bit_c) != 0) == m_bit;


        bool msb_a = ((i_state >> reg_a_msb) & 1) != 0;
        bool msb_b = ((i_state >> reg_b_msb) & 1) != 0;
        bool msb_c = ((i_state >> reg_c_msb) & 1) != 0;

        if (clk_a) {
            uint64_t s_state = (i_state & reg_a_mask) << 1;

            bool feedback = ((i_state & data_bit_a[0]) != 0) !=
                            ((i_state & data_bit_a[1]) != 0) !=
                            ((i_state & data_bit_a[2]) != 0) !=
                            ((i_state & data_bit_a[3]) != 0);

            uint64_t t_state = s_state | (uint64_t )feedback;
            i_state = (i_state & ~reg_a_mask) | (t_state & reg_a_mask);
        }
        if (clk_b) {
            uint64_t s_state = (i_state & reg_b_mask) << 1;
            bool feedback = ((i_state & data_bit_b[0]) != 0) !=
                            ((i_state & data_bit_b[1]) != 0);

            uint64_t t_state = s_state  | ((uint64_t )feedback << reg_b_lsb);
            i_state = (i_state & ~reg_b_mask) | (t_state & reg_b_mask);

        }
        if (clk_c) {
            uint64_t s_state = (i_state & reg_c_mask) << 1;
            bool feedback = ((i_state & data_bit_c[0]) != 0) !=
                            ((i_state & data_bit_c[1]) != 0) !=
                            ((i_state & data_bit_c[2]) != 0) !=
                            ((i_state & data_bit_c[3]) != 0);

            uint64_t t_state = s_state | ((uint64_t )feedback << reg_c_lsb);
            i_state = (i_state & ~reg_c_mask) | (t_state & reg_c_mask);
        }
        r_state = (r_state << 1) | (uint64_t )(msb_a != msb_b != msb_c);
    }

    return r_state;
}

uint64_t reg_1(uint64_t i_state, int rounds = 64) {
    uint64_t r_state = 0;
    uint64_t r1 = i_state & reg_a_mask;
    uint64_t r2 = (i_state & reg_b_mask) >> reg_b_lsb;
    uint64_t r3 = (i_state & reg_c_mask) >> reg_c_lsb;

    for(int i = 0; i < rounds; i++) {
        bool a = (r1 & (1UL << 8)) != 0;
        bool b = (r2 & (1UL << 10)) != 0;
        bool c = (r3 & (1UL << 10)) != 0;
        bool m_bit = (a && b) || (b && c) || (c && a);

        uint64_t msb_a = ((r1 >> (reg_a_len - 1)) & 1);
        uint64_t msb_b = ((r2 >> (reg_b_len - 1)) & 1);
        uint64_t msb_c = ((r3 >> (reg_c_len - 1)) & 1);

        uint64_t not_clock1 = ((uint64_t )(a == m_bit)) - 1;
        uint64_t not_clock2 = ((uint64_t )(b == m_bit)) - 1;
        uint64_t not_clock3 = ((uint64_t )(c == m_bit)) - 1;


//        bool feedback = ((r1 & (1UL << 13)) != 0) !=
//                        ((r1 & (1UL << 16)) != 0) !=
//                        ((r1 & (1UL << 17)) != 0) !=
//                        ((r1 & (1UL << 18)) != 0);
        uint64_t feedback = (((((r1 ^ (r1 >> 3)) ^ ((r1 ^ (r1 >> 1)) >> 4))) >> 13) & 1);

        r1 = (((r1 << 1) | feedback) & ~not_clock1) | (r1 & not_clock1);

//        feedback = ((r2 & (1UL << 20)) != 0) !=
//                        ((r2 & (1UL << 21)) != 0);
        feedback = (((r2 ^ (r2 >> 1)) >> 20) & 1);
        r2 = (((r2 << 1) | feedback) & ~not_clock2) | (r2 & not_clock2);

//        feedback = ((r3 & (1UL << 7)) != 0) !=
//                        ((r3 & (1UL << 20)) != 0) !=
//                        ((r3 & (1UL << 21)) != 0) !=
//                        ((r3 & (1UL << 22)) != 0);
        feedback = (((((r3 ^ (r3 >> 13)) ^ ((r3 ^ (r3 >> 1)) >> 14))) >> 7) & 1);
        r3 = (((r3 << 1) | (uint64_t) feedback) & ~not_clock3) | (r3 & not_clock3);

        r_state = (r_state << 1) | (msb_a ^ msb_b ^ msb_c);
    }
    return r_state;

}

int main() {
//    auto t1 = std::thread(thread_1);
//    auto t2 = std::thread(thread_2, 0);
//    t1.join();
//    t2.join();
    uint64_t op = 0;
    uint64_t r = test;
    uint64_t start = rdtsc();
    const int repeat = 11114096;
    for (int i = 0; i < repeat; i++) {
        r = reg_1(r);
    }
    uint64_t end = rdtsc();
    op = end - start;

    std::cout << std::dec << op/repeat << std::endl;
    std::cout << std::dec << op << std::endl;
    std::cout << std::hex << r << std::endl;

    r = test;
    start = rdtsc();
    for (int i = 0; i < repeat; i++) {
        r = reg_2(r);
    }
    end = rdtsc();
    op = end - start;

    std::cout << std::dec << op/repeat << std::endl;
    std::cout << std::dec << op << std::endl;
    std::cout << std::hex << r << std::endl;
    return 0;
}
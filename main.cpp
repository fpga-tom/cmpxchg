#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include "Signal.h"
#include "Sync.h"
#include "Rx.h"
#include "RxBuilder.h"
#include "DummySink.h"
#include "FmDemod.h"
#include "PaAudio.h"

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


const uint64_t fs = 0x8000;
double T = 1e6/fs;
Signal sig;
//volatile std::vector<uint64_t > buf(fs);
volatile uint64_t buf[fs];
void thread_1() {
//    uint64_t start = rdtsc();
    struct timespec start, end;
    uint64_t delta_sum = 0;

    for (uint64_t k = 0; k < 1000000; k++) {

        int64_t op = 0;

        for (uint64_t i = 0; i < fs; i++) {
            buf[i] = i*(k+1);
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        sig.put(buf);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
        delta_sum += delta_us;
        if(k % 10000 == 0)
        std::cout << "done: " << delta_us / 1000 << " " << (1000000 * fs * k)/(delta_sum+1)/(1<<30) << std::endl;
//        std::cout << op / fs << std::endl;
    }
}

void thread_2(int tid) {
    uint64_t local_var = 0;
    std::ofstream of{"/tmp/file.txt", std::ios::binary};
//    std::vector<uint64_t> *buf;
    struct timespec start, end;
    uint64_t last_timestamp = 0;
    volatile uint64_t *buf;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
//    while(!tas(&shared_var, 0));
    for(uint64_t k = 0; k < 1000000; k++) {
        buf = sig.get();
    }
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

uint64_t reg_0(uint64_t i_state, int rounds = 64) {
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

        if (a == m_bit) {
            uint64_t feedback = (((((r1 ^ (r1 >> 3)) ^ ((r1 ^ (r1 >> 1)) >> 4))) >> 13) & 1);
            r1 = ((r1 << 1) | feedback);
        }

        if(b == m_bit) {
            uint64_t feedback = (((r2 ^ (r2 >> 1)) >> 20) & 1);
            r2 = ((r2 << 1) | feedback);
        }

        if(c == m_bit) {
            uint64_t feedback = (((((r3 ^ (r3 >> 13)) ^ ((r3 ^ (r3 >> 1)) >> 14))) >> 7) & 1);
            r3 = ((r3 << 1) | feedback);
        }

        r_state = (r_state << 1) | (msb_a ^ msb_b ^ msb_c);
    }
    return r_state;

}

int main() {
#if 0
    auto t1 = std::thread(thread_1);
    auto t2 = std::thread(thread_2, 0);
    t1.join();
    t2.join();
#endif

    RXBuilder builder;
    std::unique_ptr<Rx> rx = builder
            .with_rfport("A_BALANCED")
            .with_uri("192.168.4.10")
            .enable_chn("voltage0")
            .enable_chn("voltage1")
            .enable_bbdc(true)
            .enable_rfdc(true)
            .enable_quadrature(true)
            .gain_mode("fast_attack")
            .with_lo(88.8)
            .with_bw(.2)
            .with_fs(3.072)
            .with_K(0x10000);
    FmDemod fm(0x10000,16,4);
    PaAudio pa(0x10000/64);
//    Sync s(0x8000);
//    DummySink ds;
//    s[module::sync::port::correlate::p_in].bind(rx->operator[](module::rx::port::convert ::p_out));
    fm[module::fm_demod::port::downsample_first ::p_in].bind(rx->operator[](module::rx::port::convert ::p_out));
    pa[module::pa_audio::port::play::p_in].bind(fm[module::fm_demod::port::downsample_second::p_out]);
//    ds[module::dummysink::port::sink::p_in].bind(fm[module::fm_demod::port::downsample_second ::p_out]);

    rx->start_rx();
    rx->start();
//    s3.start();
//    s2.start();
//    s1.start();
//    s.start();
    fm.start();
    pa.start();
//    ds.start();
//    s1.join();
//    s2.join();
//    s3.join();
//    ds.join();
    pa.join();
    rx->join();
    fm.join();
//    s.join();


//    uint64_t i = 0;
//    uint64_t rdd = rdtsc();
//    for(; i < 0xffffffffffffffffUL; i++) {
//        rdd += rdtsc();
//        if(i % 0xffffffff == 0) {
//            std::cout << std::hex << rdd << std::endl;
//        }
//    }

#if 0
    for(int rr = 0; rr < 1; rr++) {
        uint64_t op = 0;
        uint64_t r = test;
        struct timespec clock_start, clock_end;
        clock_gettime(CLOCK_MONOTONIC_RAW, &clock_start);
        uint64_t start = rdtsc();
        const int repeat = 11114096;
        uint64_t dp_count = 0;
        uint64_t dp_len = 0;
        uint64_t last_dp = 0;
        for (uint64_t i = 0; i < repeat; i++) {
            r = reg_0(r);
            if((r & ((1<<18)-1)) == 0) {
                std::cout << "dp " << std::hex << r << std::endl;
                ++dp_count;
                dp_len += (i - last_dp);
                last_dp = i;
            }
        }
        uint64_t end = rdtsc();
        clock_gettime(CLOCK_MONOTONIC_RAW, &clock_end);
        uint64_t delta_us = (clock_end.tv_sec - clock_start.tv_sec) * 1000000 + (clock_end.tv_nsec - clock_start.tv_nsec) / 1000;
        op = end - start;

        std::cout << std::dec << " dp len: " << (dp_len / dp_count) << std::endl;
        std::cout << std::dec << (repeat*1e6/delta_us) << std::endl;
        std::cout << std::dec << op / repeat << std::endl;
        std::cout << std::dec << op << std::endl;
        std::cout << std::hex << r << std::endl;

        r = test;
        start = rdtsc();
        for (int i = 0; i < repeat; i++) {
            r = reg_1(r);
        }
        end = rdtsc();
        op = end - start;

        std::cout << std::dec << op / repeat << std::endl;
        std::cout << std::dec << op << std::endl;
        std::cout << std::hex << r << std::endl;

        r = test;
        start = rdtsc();
        for (int i = 0; i < repeat; i++) {
            r = reg_2(r);
        }
        end = rdtsc();
        op = end - start;

        std::cout << std::dec << op / repeat << std::endl;
        std::cout << std::dec << op << std::endl;
        std::cout << std::hex << r << std::endl;
    }
#endif
    return 0;
}
//
// Created by tomas on 11/30/18.
//

#ifndef CMPXCHG_RX_H
#define CMPXCHG_RX_H

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iio.h>
#include <complex>
#include "Module.h"

namespace module {
    namespace rx {
        enum class tsk : uint8_t {generate, convert, SIZE};

        namespace port {
            enum class generate : uint8_t {p_out_r, p_out_i, SIZE};
            enum class convert : uint8_t {p_in_r, p_in_i, p_out, SIZE};
        }
    }
}

class Rx : public Module {

    inline Task&   operator[](const module::rx::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::rx::port::generate p) { return Module::operator[]((int)module::rx::tsk::generate)[(int)p]; }
    inline Port& operator[](const module::rx::port::convert p) { return Module::operator[]((int)module::rx::tsk::convert)[(int)p]; }



    std::string uri;
    std::string device = "cf-ad9361-lpc";
    std::string device_phy = "ad9361-phy";

    struct iio_context *ctx;
    struct iio_device *dev;
    struct iio_device *phy;
    uint64_t nb_channels;

    std::vector<std::string> params;
    std::vector<std::string> channels;

    void set_params(struct iio_device *phy,
                    const std::vector<std::string> &params);

    void channel_read(const struct iio_channel *chn,
                      void *dst, size_t len);

    void refill_thread();

    std::mutex iio_mutex;
    std::condition_variable iio_cond, iio_cond2;
    unsigned long items_in_buffer;

    std::thread refill_thd;

    unsigned long timeout;
    struct iio_buffer *buf;
    off_t byte_offset;
    volatile bool please_refill_buffer, thread_stopped;
    unsigned int buffer_size = 0x8000;
    std::vector <struct iio_channel *> channel_list;
    const int K;

public:

    Port& p_out() { return this->operator[](module::rx::port::convert ::p_out); }
    Rx(const int K,const std::string uri,
       const std::vector<std::string> &params,
       const std::vector<std::string> &channels,
       const int n_frames = 1) : lo_mhz(-1), K(K), uri(uri),
                                 params(params), channels(channels), items_in_buffer(0),
                                 buffer_size(K*n_frames) {

        Task & t_generate = create_task("generate", {
            TagPortOut("p_out_r", (uint8_t )module::rx::port::generate ::p_out_r, K*n_frames*sizeof(uint16_t)),
            TagPortOut("p_out_i", (uint8_t )module::rx::port::generate ::p_out_i, K*n_frames*sizeof(uint16_t)),
        }, [this](uint8_t** d_in, uint8_t **d_out) -> int { this->_generate(d_out[0], d_out[1]); return 0; });


        Task & t_convert = create_task("convert", {
            TagPortIn("p_in_r", (uint8_t)module::rx::port::convert::p_in_r),
            TagPortIn("p_in_i", (uint8_t)module::rx::port::convert::p_in_i),
            TagPortOut("p_out", (uint8_t)module::rx::port::convert::p_out,
                    K*n_frames*sizeof(std::complex<float>)),
        },[this](uint8_t **d_in, uint8_t **d_out) -> int { this->convert(d_in[0], d_in[1], d_out[0]); return 0; });


        this->operator[](module::rx::port::convert::p_in_r).bind(
                this->operator[](module::rx::port::generate ::p_out_r));
        this->operator[](module::rx::port::convert::p_in_i).bind(
                this->operator[](module::rx::port::generate ::p_out_i));
    }



    virtual ~Rx() = default;

    void start_rx();
    void tune(uint32_t);
    int32_t lo_mhz;
protected:
    void _generate(uint8_t *real, uint8_t *imag);

    void convert(uint8_t *d_in_r, uint8_t *d_in_i, uint8_t *_d_out) {
        int16_t *real = (int16_t *)d_in_r;
        int16_t *imag = (int16_t *)d_in_i;
        std::complex<float> *d_out = (std::complex<float>*)_d_out;
        for(int i = 0; i < K; i++) {
            d_out[i] = std::complex<float>{(float)real[i], (float)imag[i]};
        }
    }
};


#endif //CMPXCHG_RX_H

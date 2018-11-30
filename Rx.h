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
#include "Module.h"

namespace module {
    namespace rx {
        enum class tsk : uint8_t {generate, SIZE};

        namespace port {
            enum class generate : uint8_t {
                p_out, SIZE
            };
        }
    }
}

class Rx : public Module {

    inline Task&   operator[](const module::rx::tsk           t) { return Module::operator[]((int)t);                          }


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
    Rx(const int K,const std::string uri,
       const std::vector<std::string> &params,
       const std::vector<std::string> &channels,
       const int n_frames = 1) : K(K), uri(uri),
                                 params(params), channels(channels), items_in_buffer(0),
                                 buffer_size(K*n_frames) {
        Task & t_generate = create_task("generate");

        Port & p_generate = t_generate.create_port_out("p_out", K*n_frames*sizeof(uint16_t));

        t_generate.create_codelet([this]() -> int {
            Port& p = this->operator[](module::rx::port::generate ::p_out);
            for(uint64_t i= 0; true ;i++) {
                std::shared_ptr<std::vector<uint8_t >> v_out = this->operator[](module::rx::tsk::generate).buf[i%2];

                _generate(v_out->data());

                p.put(reinterpret_cast<uint64_t>(v_out->data()));
            }

            return 0;
        });
    };

    virtual ~Rx() = default;

    void start_rx();
    inline Port& operator[](const module::rx::port::generate p) { return Module::operator[]((int)module::rx::tsk::generate)[(int)p]; }
protected:
    void _generate(uint8_t *U_K);
};


#endif //CMPXCHG_RX_H

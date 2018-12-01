//
// Created by tomas on 11/30/18.
//

#include "Rx.h"


//
// Created by tomas on 11/17/18.
//

#include <cstring>
#include <chrono>
#include <iostream>


#define ASSERT(expr) { \
	if (!(expr)) { \
		(void) fprintf(stderr, "assertion failed (%s:%d)\n", __FILE__, __LINE__); \
		(void) abort(); \
	} \
}

void Rx::start_rx() {
    printf("* Acquiring IIO context\n");
    ASSERT((ctx = iio_create_network_context(uri.c_str())) && "No context");
    ASSERT(iio_context_get_devices_count(ctx) > 0 && "No devices");

    dev = iio_context_find_device(ctx, device.c_str());
    phy = iio_context_find_device(ctx, device_phy.c_str());
    if (!dev || !phy) {
        iio_context_destroy(ctx);
        throw std::runtime_error("Device not found");
    }

    nb_channels = iio_device_get_channels_count(dev);
    for (int i = 0; i < nb_channels; i++)
        iio_channel_disable(iio_device_get_channel(dev, i));

    for (std::vector<std::string>::const_iterator it =
            channels.begin();
         it != channels.end(); ++it) {
        struct iio_channel *chn =
                iio_device_find_channel(dev,
                                        it->c_str(), false);
        if (!chn) {
            iio_context_destroy(ctx);
            throw std::runtime_error(
                    "Channel not found");
        }

        iio_channel_enable(chn);
        channel_list.push_back(chn);
    }

    set_params(phy, params);

    std::unique_lock<std::mutex> lock(iio_mutex);

    items_in_buffer = 0;
    please_refill_buffer = false;
    thread_stopped = false;

    buf = iio_device_create_buffer(dev, buffer_size, false);

    if (buf) {
        refill_thd = std::thread(
                &Rx::refill_thread, this);
    } else {
        throw std::runtime_error("Unable to create buffer!\n");
    }

}

void Rx::set_params(struct iio_device *phy,
                       const std::vector<std::string> &params)
{
    for (std::vector<std::string>::const_iterator it = params.begin();
         it != params.end(); ++it) {
        struct iio_channel *chn = NULL;
        const char *attr = NULL;
        size_t pos;
        int ret;

        pos = it->find('=');
        if (pos == std::string::npos) {
            std::cerr << "Misformed line: " << *it << std::endl;
            continue;
        }

        std::string key = it->substr(0, pos);
        std::string val = it->substr(pos + 1, std::string::npos);

        ret = iio_device_identify_filename(phy,
                                           key.c_str(), &chn, &attr);
        if (ret) {
            std::cerr << "Parameter not recognized: "
                      << key << std::endl;
            continue;
        }

        if (chn)
            ret = iio_channel_attr_write(chn,
                                         attr, val.c_str());
        else if (iio_device_find_attr(phy, attr))
            ret = iio_device_attr_write(phy, attr, val.c_str());
        else
            ret = iio_device_debug_attr_write(phy,
                                              attr, val.c_str());
        if (ret < 0) {
            std::cerr << "Unable to write attribute " << key
                      <<  ": " << ret << " " << strerror(errno) << std::endl;
        }
    }
}




void
Rx::refill_thread()
{
    std::unique_lock<std::mutex> lock(iio_mutex);
    ssize_t ret;

    for (;;) {
        while (!please_refill_buffer) {
            auto fast_enough = iio_cond.wait_for(lock, std::chrono::milliseconds(100));
            if (fast_enough == std::cv_status::timeout) {
                std::cerr << "owerflow" << std::endl;
            }
        }


        please_refill_buffer = false;

        lock.unlock();
        ret = iio_buffer_refill(buf);
        lock.lock();

        if (ret < 0)
            break;

        items_in_buffer = (unsigned long) ret / iio_buffer_step(buf);
        byte_offset = 0;

        iio_cond2.notify_all();
    }

    /* -EBADF happens when the buffer is cancelled */
    if (ret != -EBADF) {

        char buf[256];
        iio_strerror(-ret, buf, sizeof(buf));
        std::string error(buf);

        std::cerr << "Unable to refill buffer: " << error << std::endl;
    }

    thread_stopped = true;
    iio_cond2.notify_all();
}

void Rx::channel_read(const struct iio_channel *chn,
                         void *dst, size_t len)
{
    uintptr_t src_ptr, dst_ptr = (uintptr_t) dst, end = dst_ptr + len;
    unsigned int length = iio_channel_get_data_format(chn)->length / 8;
    uintptr_t buf_end = (uintptr_t) iio_buffer_end(buf);
    ptrdiff_t buf_step = iio_buffer_step(buf);

    for (src_ptr = (uintptr_t) iio_buffer_first(buf, chn) + byte_offset;
         src_ptr < buf_end && dst_ptr + length <= end;
         src_ptr += buf_step, dst_ptr += length) {
//        const int16_t i = ((int16_t*)src_ptr)[0]; // Real (I)
//        const int16_t q = ((int16_t*)src_ptr)[1]; // Imag (Q)
//        ((int16_t*)dst_ptr)[0] = q;
//        ((int16_t*)dst_ptr)[1] = i;
        iio_channel_convert(chn,
                            (void *) dst_ptr, (const void *) src_ptr);
    }
}

void Rx::_generate(uint8_t *real, uint8_t *imag) {

    std::unique_lock<std::mutex> lock(iio_mutex);

    if (!please_refill_buffer && !items_in_buffer) {
        please_refill_buffer = true;
        iio_cond.notify_all();
    }

    while (please_refill_buffer) {
        auto fast_enough = iio_cond2.wait_for(lock, std::chrono::milliseconds(500));
        if(fast_enough == std::cv_status::timeout) {
            std::cerr << "timout reading data" << std::endl;
            return;
        }
        if (thread_stopped)
            return; /* EOF */
    }

    uint32_t items = this->K;
    channel_read(channel_list[0], real,
                 items * sizeof(short));
    channel_read(channel_list[1], imag,
                 items * sizeof(short));

    items_in_buffer -= items;
    byte_offset += items * iio_buffer_step(buf);


}

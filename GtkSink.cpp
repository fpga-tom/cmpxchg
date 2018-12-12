//
// Created by tomas on 12/12/18.
//

#include "GtkSink.h"

GtkSink::GtkSink(const int buf_size, GtkProto &proto) : buf_size(buf_size), proto(proto), acc(buf_size) {

    for(int i = 0;i < buf_size; i++) {
        acc[i] = (0);
    }

    for(int j = 0; j < 5; j++) {
        std::vector<float> tmp;
        for (int i = 0; i < buf_size; i++) {
            tmp.emplace_back(0);
        }
        vec_history.emplace_back(tmp);
    }


    Task & t_sdl = create_task("gtk", {
            TagPortIn("p_in", (uint8_t )module::gtk::port::paint ::p_in)
    },[this, buf_size, &proto](uint8_t** d_in, uint8_t **d_out) -> int {
        fps.start();
        float * d = reinterpret_cast<float *>(d_in[0]);
        std::vector<float> x_vec(buf_size);
        std::vector<float> d_vec(buf_size);
        std::vector<float> delay = vec_history.front();
        for(int i = 0; i < buf_size; i++) {
            acc[i] += d[i] - delay[i];
            d_vec[i] = d[i];
            x_vec[i] = (acc[i]/5.);
        }
        float sum = 0;
        for(int i = 0; i < buf_size; i++) {
            sum += x_vec[i];
        }
        float mean = sum / buf_size;
        for(int i = 0; i < buf_size; i++) {
            x_vec[i] -= mean - 9;
            x_vec[i] *= 3;
        }
        vec_history.pop_front();
        vec_history.emplace_back(d_vec);
        proto.x_range(0, 1024);
        proto.y_range(0, 130);
        proto.send(x_vec);

        //Cap the frame rate
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() ));
        }
        return 0;

    });
}

//
// Created by tomas on 12/2/18.
//

#include "GnuplotSink.h"
#include "gnuplot_i.hpp"

Gnuplot gp("gnuplot");
GnuplotSink::GnuplotSink(const int buf_size) : buf_size(buf_size) {

    Task & t_sdl = create_task("gnuplot", {
            TagPortIn("p_in", (uint8_t )module::gnuuplot::port::paint ::p_in)
    },[this, buf_size](uint8_t** d_in, uint8_t **d_out) -> int {
        fps.start();
        float * d = reinterpret_cast<float *>(d_in[0]);
        std::vector<std::pair<float, float>> x_vec(buf_size);
        for(int i = 0; i < buf_size; i++) {
            x_vec[i] = std::make_pair(i, d[i]);
        }
        gp << "set xrange [0:1024]\nset yrange [0:130]\nset grid\n";
        gp << "plot '-' with lines\n";

        gp.send1d(x_vec);

        //Cap the frame rate
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() ));
        }
        return 0;

    });
}

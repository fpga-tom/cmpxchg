//
// Created by tomas on 12/2/18.
//

#ifndef CMPXCHG_GNUPLOTSINK_H
#define CMPXCHG_GNUPLOTSINK_H

#include <cstdint>
#include "Module.h"
#include "Timer.h"

namespace module {
    namespace gnuuplot {
        enum class tsk : uint8_t {paint, SIZE};

        namespace port {
            enum class paint : uint8_t {
                p_in, SIZE
            };
        }
    }
}


class GnuplotSink : public Module {
    inline Task&   operator[](const module::gnuuplot::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::gnuuplot::port::paint p) { return Module::operator[]((int)module::gnuuplot::tsk::paint)[(int)p]; }
    const int buf_size;
    const int FRAMES_PER_SECOND = 10;

    Timer fps;
public:
    Port& p_in() { return this->operator[](module::gnuuplot::port::paint::p_in); }
    GnuplotSink(const int buf_size);


};


#endif //CMPXCHG_GNUPLOTSINK_H

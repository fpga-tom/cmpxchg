//
// Created by tomas on 12/12/18.
//

#ifndef CMPXCHG_GTKSINK_H
#define CMPXCHG_GTKSINK_H


#include <cstdint>
#include <deque>
#include "Module.h"
#include "Timer.h"

namespace module {
    namespace gtk {
        enum class tsk : uint8_t {paint, SIZE};

        namespace port {
            enum class paint : uint8_t {
                p_in, SIZE
            };
        }
    }
}

class GtkProto {
public:
    virtual void x_range(uint64_t from, uint64_t to) = 0;
    virtual void y_range(uint64_t from, uint64_t to) = 0;
    virtual void send(std::vector<float> data) = 0;
};


class GtkSink : public Module {
    inline Task&   operator[](const module::gtk::tsk           t) { return Module::operator[]((int)t);                          }
    inline Port& operator[](const module::gtk::port::paint p) { return Module::operator[]((int)module::gtk::tsk::paint)[(int)p]; }
    const int buf_size;
    const int FRAMES_PER_SECOND = 10;
    GtkProto &proto;
    std::deque<std::vector< float>> vec_history;
    std::vector< float> acc;

    Timer fps;
public:
    Port& p_in() { return this->operator[](module::gtk::port::paint::p_in); }
    GtkSink(const int buf_size, GtkProto &proto);


};

#endif //CMPXCHG_GTKSINK_H

//
// Created by tomas on 12/1/18.
//

#ifndef CMPXCHG_PAAUDIO_H
#define CMPXCHG_PAAUDIO_H

#include <cstdint>
#include <pulse/simple.h>
#include <pulse/error.h>
#include "Module.h"

namespace module {
    namespace pa_audio {
        enum class tsk : uint8_t {play, SIZE};

        namespace port {
            enum class play : uint8_t {
                p_in, SIZE
            };
        }
    }
}


class PaAudio : public Module {
    inline Task&   operator[](const module::pa_audio::tsk           t) { return Module::operator[]((int)t);                          }

    int play(uint8_t** d_in, uint8_t **d_out);

    const pa_sample_spec ss = {
            .format = PA_SAMPLE_S16LE,
            .rate = 48000,
            .channels = 1
    };
    pa_simple *s = NULL;
    int error;

public:
    inline Port& operator[](const module::pa_audio::port::play p) { return Module::operator[]((int)module::pa_audio::tsk::play)[(int)p]; }


    const int buf_size;
    PaAudio(const int buf_size) : buf_size(buf_size*sizeof(int16_t)) {
        if (!(s = pa_simple_new(NULL, "pa_audio", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
            fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        }

        Task & t_play = create_task("play", {
                TagPortIn("p_in", (uint8_t )module::pa_audio::port::play::p_in)
        },[this](uint8_t** d_in, uint8_t **d_out) -> int { return this->play(d_in, d_out); });
    }


};


#endif //CMPXCHG_PAAUDIO_H

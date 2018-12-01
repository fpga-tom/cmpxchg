//
// Created by tomas on 12/1/18.
//

#ifndef CMPXCHG_SDLSINK_H
#define CMPXCHG_SDLSINK_H

#include <cmath>
#include "SDL/SDL.h"
#include "Module.h"
#include "Timer.h"

namespace module {
    namespace sdl {
        enum class tsk : uint8_t {paint, SIZE};

        namespace port {
            enum class paint : uint8_t {
                p_in, SIZE
            };
        }
    }
}


class SDLSink : public Module {

    inline Task&   operator[](const module::sdl::tsk           t) { return Module::operator[]((int)t);                          }
    const int buf_size;

    const int SCREEN_WIDTH = 1024; const int SCREEN_HEIGHT = 480; const int SCREEN_BPP = 32;
    SDL_Surface *message = NULL; SDL_Surface *background = NULL; SDL_Surface *screen = NULL;
    const int FRAMES_PER_SECOND = 30;

    Timer fps;


    void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
    {
        //Holds offsets
        SDL_Rect offset;

        //Get offsets
        offset.x = x;
        offset.y = y;

        //Blit
        SDL_BlitSurface( source, clip, destination, &offset );
    }

    bool init()
    {
        //Initialize all SDL subsystems
        if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
        {
            return false;
        }

        //Set up the screen
        screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

        //If there was an error in setting up the screen
        if( screen == NULL )
        {
            return false;
        }

        //Set the window caption
        SDL_WM_SetCaption( "cmpxchg", NULL );

        //If everything initialized fine
        return true;
    }

    std::unique_ptr<std::vector<uint32_t >> screen_pixels;
    int row;

public:
    inline Port& operator[](const module::sdl::port::paint p) { return Module::operator[]((int)module::sdl::tsk::paint)[(int)p]; }

    SDLSink(const int buf_size) : buf_size(buf_size), row(0) {
        init();
        screen_pixels = std::move(std::make_unique<std::vector<uint32_t >>(screen->h * screen->pitch));

        Task & t_sdl = create_task("sdl", {
                TagPortIn("p_in", (uint8_t )module::sdl::port::paint ::p_in)
        },[this, buf_size](uint8_t** d_in, uint8_t **d_out) -> int {
            fps.start();


//            SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
            float *d = reinterpret_cast<float *>(d_in[0]);

            SDL_LockSurface(screen);
            memcpy(&screen_pixels->data()[SCREEN_WIDTH], screen->pixels, screen_pixels->size() - SCREEN_WIDTH );
            SDL_UnlockSurface(screen);
            for(int i = 0; i < buf_size; i++) {
                float rLow = 0.0f; float gLow = 0.1f; float bLow = 0.2f;
                float rHigh = 0.13f; float gHigh = 0.53f; float bHigh = 0.2f;

                float vLow = 0;
                float vHigh = 150;

                float prop = ( d[i] - vLow) / ( vHigh - vLow );
                float rVal = (rLow + prop * ( rHigh - rLow ));
                float gVal = (gLow + prop * ( gHigh - gLow ));
                float bVal = (bLow + prop * ( bHigh - bLow ));

                uint8_t R = static_cast<uint8_t>(rVal*255);
                uint8_t G = static_cast<uint8_t>(gVal*255);
                uint8_t B = static_cast<uint8_t>(bVal*255);
                uint32_t rgb = SDL_MapRGB( screen->format, R, G, B );
                screen_pixels->operator[](i) = rgb;
//                screen_pixels->operator[](row*screen->pitch + i*4 + 1) = (uint8_t )(d[i]/256);
//                screen_pixels->operator[](row*screen->pitch + i*4 + 2) = (uint8_t )(d[i]/256/256);
//                screen_pixels->operator[](row*screen->pitch + i*4 + 3) = 10;
            }
            row = (row + 1) % SCREEN_HEIGHT;
            SDL_LockSurface(screen);
            memcpy(screen->pixels, screen_pixels->data(), screen_pixels->size() );
            SDL_UnlockSurface(screen);

            //Update the screen
            if( SDL_Flip( screen ) == -1 )
            {
                return 1;
            }

            //Cap the frame rate
            if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
            {
                SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
            }

            return 0;
        });
    }
};


#endif //CMPXCHG_SDLSINK_H

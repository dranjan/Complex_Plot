#ifndef SDL_ANIM_HPP
#define SDL_ANIM_HPP

#include "color.hpp"
#include "sdlm-v01.hpp"

#include <vector>

int play_wrapper(void *data);

class sdl_animation : public sdl_manager {
protected:
    vector<SDL_Surface *> frames;
    Uint32 delay_ms;

    friend int play_wrapper(void *data);

    SDL_Thread *play_thread;

    /* Set this to true before calling play() */
    bool playing;

    unsigned int current_frame;

    /* This isn't really the right way to do this; a better way
     * would be to use a timer to push user events onto the
     * event queue.
     */
    void play() {
        if (frames.size() > 0) {
            while (playing) {
                show_surface(frames[current_frame++]);
                if (current_frame == frames.size()) current_frame = 0;
                SDL_Delay(delay_ms);
            }
        }
    }

    void add_frame() {
        frames.push_back(new_surface());
    }

    void set_frame_rect(int f, int x0, int y0, int w, int h, color c) {
        assert( screen_init );

        SDL_Surface *s = frames[f];

        SDL_Rect rect = {x0, y0, w, h};
        Uint32 co = SDL_MapRGB(s->format, c.r*255, c.g*255, c.b*255);

        SDL_FillRect(s, &rect, co);
    }

    void set_frame_pixel(int f, int x, int y, color c) {
        set_frame_rect(f, x, y, 1, 1, c);
    }

    bool update_frame(int f) {
        if (SDL_Flip(frames.at(f)) == -1) return false;
        return true;
    }

    virtual void keydown_event(SDL_KeyboardEvent & key) {
        SDLKey sym = key.keysym.sym;
        if (sym == SDLK_ESCAPE) {
            quit_event_loop = true;
        } 
    }

public:
    void init(int w, int h) {
        init_screen(w, h);
    }

    sdl_animation() : sdl_manager() { }

    sdl_animation(int w, int h) {
        init(w, h);
    }

    void run() {
        current_frame = 0;
        playing = true;

        play_thread = SDL_CreateThread(play_wrapper, this);

        event_loop();

        playing = false;
        int status;

        SDL_WaitThread(play_thread, &status);
    }

   virtual ~sdl_animation() {
        for (unsigned int k = 0; k < frames.size(); ++k) {
            del_surface(frames[k]);
        }
    }
};

inline int play_wrapper(void *data) {
    sdl_animation *o = (sdl_animation *)data;
    o->play();
    return 0;
}

#endif

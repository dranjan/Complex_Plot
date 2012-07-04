#ifndef SDLM_H
#define SDLM_H

#include <string>
#include <csignal>
#include <cassert>

#include "SDL/SDL.h"

class sdl_manager {

protected:
    SDL_Surface *screen;

    bool screen_init;

    bool quit_event_loop;

    void init() {
        if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
            throw std::string("error initializing SDL");    
        }

        std::signal(SIGINT, SIG_DFL);
    }

    virtual void active_event(SDL_ActiveEvent & active) { }
    virtual void keydown_event(SDL_KeyboardEvent & key) { }
    virtual void keyup_event(SDL_KeyboardEvent & key) { }
    virtual void mousemotion_event(SDL_MouseMotionEvent & motion) { }
    virtual void mousebuttondown_event(SDL_MouseButtonEvent & button) { }
    virtual void mousebuttonup_event(SDL_MouseButtonEvent & button) { }
    virtual void joyaxis_event(SDL_JoyAxisEvent & jaxis) { }
    virtual void joyball_event(SDL_JoyBallEvent & jball) { }
    virtual void joyhat_event(SDL_JoyHatEvent & jhat) { }
    virtual void joybuttondown_event(SDL_JoyButtonEvent & jbutton) { }
    virtual void joybuttonup_event(SDL_JoyButtonEvent & jbutton) { }
    virtual void resize_event(SDL_ResizeEvent & resize) { }
    virtual void expose_event(SDL_ExposeEvent & expose) { }
    virtual void quit_event(SDL_QuitEvent & quit) { 
        quit_event_loop = true;
    }
    virtual void user_event(SDL_UserEvent & user) { }
    virtual void syswm_event(SDL_SysWMEvent & syswm) { }
    
public:
    void init_screen(int w, int h) {
        if (screen_init) SDL_Quit();

        screen = SDL_SetVideoMode(w, h, 32, SDL_SWSURFACE);
        if (!screen) {
            throw std::string("error setting video mode");
        }

        screen_init = true;
        quit_event_loop = false;
    }


    /* Free with del_surface */
    SDL_Surface *new_surface() {
        assert( screen_init );

        SDL_PixelFormat *fmt = SDL_GetVideoInfo()->vfmt;
        return SDL_CreateRGBSurface(SDL_SWSURFACE, 
                                    screen->w, screen->h, 32, 
                                    fmt->Rmask, fmt->Gmask, fmt->Bmask,
                                    fmt->Amask);
    }

    bool show_surface(SDL_Surface *surface) {
        assert( screen_init );

        SDL_BlitSurface(surface, NULL, screen, NULL);
        if (SDL_Flip(screen) == -1) return false;
        return true;
    }

    void del_surface(SDL_Surface *surface) {
        SDL_FreeSurface(surface);
    }

    sdl_manager() : screen_init(false) {
        init();
    }

    sdl_manager(int w, int h) : screen_init(false) {
        init();
        init_screen(w, h);
    }

    virtual ~sdl_manager() {
        SDL_Quit();
    }

    void set_title(const char *s) {
        SDL_WM_SetCaption(s, NULL);
    }

    bool update() {
        if (SDL_Flip(screen) == -1) return false;
        return true;
    }

    /* This will not update the screen; call update() after
     * all changes have been made 
     */
    void set_rect(Sint16 x0, Sint16 y0, Uint16 w, Uint16 h,
                  double r, double g, double b)
    {
        assert( screen_init );

        SDL_Rect rect = {x0, y0, w, h};
        Uint32 color = SDL_MapRGB(screen->format, r*255, g*255, b*255);

        SDL_FillRect(screen, &rect, color);
    }

    void set_pixel(int x, int y, double r, double g, double b) {
        set_rect(x, y, 1, 1, r, g, b);
    }

    void set_pixel_int(int x, int y, Uint8 r, Uint8 g, Uint8 b) {
        assert( screen_init );

        Uint32 color = SDL_MapRGB(screen->format, r, g, b);

        SDL_LockSurface(screen);
        Uint32 *pixels = (Uint32 *)(screen->pixels);
        pixels[y*screen->w + x] = color;
        SDL_UnlockSurface(screen);
    }

    /* This will change the pixel and update the screen */
    void draw_pixel(int x, int y, double r, double g, double b) {
        set_pixel(x, y, r, g, b);

        SDL_UpdateRect(screen, x, y, 1, 1);
    }
    
    virtual void event_loop() {
        SDL_Event event;

        quit_event_loop = false;

        while (!quit_event_loop) {
            if (SDL_WaitEvent(&event)) {
                switch (event.type) {
                case SDL_ACTIVEEVENT:
                    active_event(event.active);
                    break;
                case SDL_KEYDOWN:
                    keydown_event(event.key);
                    break;
                case SDL_KEYUP:
                    keyup_event(event.key);
                    break;
                case SDL_MOUSEMOTION:
                    mousemotion_event(event.motion);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mousebuttondown_event(event.button);
                    break;
                case SDL_MOUSEBUTTONUP:
                    mousebuttonup_event(event.button);
                    break;
                case SDL_JOYAXISMOTION:
                    joyaxis_event(event.jaxis);
                    break;
                case SDL_JOYBALLMOTION:
                    joyball_event(event.jball);
                    break;
                case SDL_JOYHATMOTION:
                    joyhat_event(event.jhat);
                    break;
                case SDL_JOYBUTTONDOWN:
                    joybuttondown_event(event.jbutton);
                    break;
                case SDL_JOYBUTTONUP:
                    joybuttonup_event(event.jbutton);
                    break;
                case SDL_VIDEORESIZE:
                    resize_event(event.resize);
                    break;
                case SDL_VIDEOEXPOSE:
                    expose_event(event.expose);
                    break;
                case SDL_QUIT:
                    quit_event(event.quit);
                    break;
                case SDL_USEREVENT:
                    user_event(event.user);
                    break;
                case SDL_SYSWMEVENT:
                    syswm_event(event.syswm);
                    break;
                default:
                    break;
                }
            }
        }
    }
};

#endif

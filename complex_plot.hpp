#ifndef COMPLEX_PLOT_HPP
#define COMPLEX_PLOT_HPP

#include <string>
#include <complex>
#include <cmath>
#include <iostream>
#include <utility>
#include <cstdlib>

#include "sdlm-v01.hpp"
#include "color.hpp"

#include "SDL/SDL.h"

typedef std::complex<double> cplx;

/* General utilities *************************************************/

/* Proper mod function, which seems to be completely absent
 * from the standard library.
 */
inline double mod(double x, double y) {
    return x - y*std::floor(x/y);
}

/* Round a double to the nearest integer.
 * (It seems the `round' function isn't in standard C++?)
 */
inline int dtoi(double x) {
    return std::floor(x+0.5);
}

/* Standard color maps ***********************************************/

inline color basic_colormap(cplx z) {
    double n(std::norm(z));
    
    n = std::sqrt(n);
    n = (n-1)/(n+1);
    double d(std::acos(n)/M_PI);

    return HSLcolor(std::arg(z), 1.0, d);
}

inline color var_colormap(cplx z) {
    bool r = std::norm(z) <= 1;
    bool g = z.real() >= 0;
    bool b = z.imag() >= 0;

    return RGBcolor(1.0*r, 1.0*g, 1.0*b);
}

inline color checkermap(cplx z) {
    double x = std::floor(mod(z.real(), 2.0));
    double y = std::floor(mod(z.imag(), 2.0));
    double d = int(x + y) % 2;

    return RGBcolor(d, d, d);
    //return HSLcolor(0, 0, int(x + y) % 2);
}

inline color halfmap(cplx z) {
    if (z.imag() >= 0) return RGBcolor(0, 0, 0);
    else return RGBcolor(1.0, 1.0, 1.0);
}

/* The plot interface ************************************************/

/* Wrappers for plotting & updating the screen
 * (so other threads can do them).
 */
int plot_wrapper(void *data);
int update_wrapper(void *data);

class complex_plot : public sdl_manager {

protected:
    /* These define the plotted region 
     * (along with the width and height)
     */
    cplx ul;
    double res;

public:
    /* Modify this to change the name of the
     * BMP dump (obtained by pressing 'S' in the
     * SDL window).
     */
    string bmp_name;

    /* > 0 means stuff is printed; <= 0 means
     * nothing is printed.
     */
    int verbosity;

    /* number of milliseconds to wait between
     * screen updates.
     */
    int delay;

    void dump_window(const char *name) {
        SDL_SaveBMP(screen, name);
        if (verbosity > 0) {
            std::cout << "Window dumped to `" << name << "'."
                      << std::endl;
        }
    }

protected:
    cplx to_plane(double x, double y) {
        return ul + res*cplx(x, -y);
    }

    pair<int, int> from_plane(cplx z) {
        cplx disp = z - ul;
        int x = (int)(floor(disp.real()/res));
        int y = (int)(floor(-disp.imag()/res));

        return pair<int, int>(x, y);
    }

    /* The next three functions are all meant to be
     * overridden by derived classes.
     */
    virtual cplx transform(cplx z) {
        return z;
    }

    virtual color colormap(cplx z) {
        return basic_colormap(z);
    }

    virtual color color_func(cplx z) {
        return colormap(transform(z));
    }

    friend int plot_wrapper(void *data);
    friend int update_wrapper(void *data);

    SDL_Thread *plot_thread;
    SDL_Thread *update_thread;

    /* sres is the subsample resolution; it must be set before
     * calling plot()
     */
    int sres;
    bool working;

    int plot() {
        assert( screen_init );

        int num_pixels = screen->w * screen->h;
        int percent_complete = 0;

        if (verbosity > 0) std::cout << "0% complete.";

        for (int y = 0; y < screen->h; ++y) {
            for (int x = 0; x < screen->w; ++x) {
                if (!working) return 1;

                int pixel = y*screen->w + x;

                color cval = RGBcolor(0, 0, 0);

                for (int k = 0; k < sres; ++k) {
                    for (int j = 0; j < sres; ++j) {
                        double sx = (k+0.5)/sres;
                        double sy = (j+0.5)/sres;

                        cval += color_func(to_plane(x+sx, y+sy));
                    }
                }

                cval /= sres * sres;
                set_pixel(x, y, cval.r, cval.g, cval.b);

                int new_percent = 100*(pixel + 1)/num_pixels;
                if (new_percent > percent_complete) {
                    percent_complete = new_percent;
                    if (verbosity > 0) {
                        std::cout << "\r" << percent_complete 
                                  << "% complete.";
                        std::cout.flush();
                    }
                }
            }
        }

        working = false;

        if (verbosity > 0) std::cout << std::endl;

        update();

        return 0;
    }

    void replot(bool blackout=true) {
        working = false;
        SDL_WaitThread(plot_thread, 0);
        SDL_KillThread(update_thread);

        if (blackout) {
            set_rect(0, 0, screen->w, screen->h, 0, 0, 0);
        }
        update();

        working = true;
        plot_thread = SDL_CreateThread(plot_wrapper, this);
        update_thread = SDL_CreateThread(update_wrapper, this);
    }

    virtual void keydown_event(SDL_KeyboardEvent & key) {
        SDLKey sym = key.keysym.sym;
        if (sym == SDLK_ESCAPE) {
            quit_event_loop = true;
        } else if (sym == SDLK_s) {
            dump_window(bmp_name.c_str());
        } else if (sym == SDLK_o) {
            recenter(1/(3.0*res), 
                     ul + res*cplx((screen->w + 1)/2.0,
                                  -(screen->h + 1)/2.0));
            replot();
        }
    }

    virtual void mousebuttondown_event(SDL_MouseButtonEvent & button) {
        Uint16 x = button.x;
        Uint16 y = button.y;
        
        cplx c = to_plane(x+0.5, y+0.5);

        if (button.button == SDL_BUTTON_MIDDLE) {
            recenter(3/res, c);
            if (verbosity > 0) {
                std::cout << "Recentering: " << c
                          << ", res. = " << res << std::endl;
            }
            replot();
        } else {
            if (verbosity > 0) {
                std::cout << c << std::endl;
            }
        }
    }

    void set_defaults() {
        bmp_name = "dump.bmp";
        verbosity = 1;
        delay = 1000;

        plot_thread = 0;
        update_thread = 0;
    }

public:
    void recenter(double pixel_res, cplx center) {
        res = 1.0/pixel_res;
        ul = center + res*cplx(-(screen->w + 1)/2, (screen->h + 1)/2);
    }

    /* For convenience, the window can be set after the object 
     * is constructed. (Just don't forget to do it!) 
     */

    /* This is the canonical way to set the display window, but
     * it sometimes isn't the most convenient.
     */
    void set_window(double real_sz, double imag_sz, double pixel_res,
                    cplx center = 0.0)
    {
        int w = dtoi(real_sz*pixel_res);
        int h = dtoi(imag_sz*pixel_res);

        init_screen(w, h);
        recenter(pixel_res, center);
    }

    /* It's often more convenient to pick the width and height in pixels
     * and either the width or height in real units. 
     */
    void width_set_window(int w, int h, double wdim, cplx center = 0.0)
    {
        set_window(wdim, (wdim*h)/w, w/wdim, center);
    }

    void height_set_window(int w, int h, double hdim, cplx center = 0.0)
    {
        set_window((hdim*w)/h, hdim, h/hdim, center);
    }

    complex_plot(double real_sz, double imag_sz, double pixel_res,
                 cplx center = 0.0)
    {
        set_defaults();
        set_window(real_sz, imag_sz, pixel_res, center);
    }

    complex_plot()  {
        set_defaults();
    }

    virtual ~complex_plot() {
    }

    void run(int subsample_res = 1) {
        sres = subsample_res;

        working = true;

        plot_thread = SDL_CreateThread(plot_wrapper, this);
        update_thread = SDL_CreateThread(update_wrapper, this);

        event_loop();

        working = false;
        int status;

        SDL_WaitThread(plot_thread, &status);

        if (status && (verbosity > 0)) {
            std::cout << "\n[plot terminated]" << std::endl;
        }

        SDL_KillThread(update_thread);
    }
};

inline int plot_wrapper(void *data) {
    complex_plot *cp = (complex_plot *)data;

    return cp->plot();
}

inline int update_wrapper(void *data) {
    complex_plot *cp = (complex_plot *)data;

    while (cp->working) {
        SDL_Delay(1000);
        cp->update();
    }

    return 0;
}

#endif

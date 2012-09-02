#ifndef MANDELMOIRE01_HPP
#define MANDELMOIRE01_HPP 

#include <iostream>
#include <cstdlib>
#include "complex_plot_var01.hpp"
#include "angle_tools.hpp"

class mandelmoire01 : public complex_plot_var01 {
protected:
    int max_iter;

    color color_func(cplx z) {
        cplx w(0.0);
        int k;

        for (k = 0; k < max_iter; ++k) {
            w = w*w + z;
            if (std::abs(w) > 1e6) break;
        }
        if (k == max_iter) --k;
        
        double t = repdbl(arg(w), max_iter-k);
        
        return HSVcolor(t, 1.0, 1.0);
    }

    virtual void keydown_event(SDL_KeyboardEvent & key) {
        SDLKey sym = key.keysym.sym;
        if (sym == SDLK_UP) {
            ++max_iter;
            std::cout << "max_iter = " << max_iter << std::endl;
            replot();
        } else if (sym == SDLK_DOWN) {
            --max_iter;
            replot();
        } else {
            complex_plot_var01::keydown_event(key);
        }
    }

    void init(int n) {
        max_iter = n;
    }

public:
    mandelmoire01(int n = 0) : max_iter(n)
    { }
};

#endif

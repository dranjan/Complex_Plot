#include "complex_plot_var01.hpp"
#include "angle_tools.hpp"
#include <cstdlib>

using namespace std;

class mandelmoire02 : public complex_plot_var01 {
protected:
    int max_iter;

    color color_func(cplx z) {
        cplx w(0.0);
        int k;
        double d = 0;

        for (k = 0; k < max_iter; ++k) {
            w = w*w + z;
            d = abs(w);
            if (d > 1e6) break;
        }
        if (k == max_iter) --k;
        
        double t = repdbl(arg(w), max_iter-k);
        double u = log(d)*pow(2, max_iter-k);
        
        if (d < 0) {
            return RGBcolor(0, 0, 0);
        } else {
            return HSVcolor(t, 1.0, 1.0)/(1+log2(1+u)/10);
        }
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
        } else if (sym == SDLK_UP) {
            ++max_iter;
            replot();
        } else if (sym == SDLK_DOWN) {
            --max_iter;
            replot();
        }
    }

public:
    mandelmoire02(int n) : complex_plot_var01(8.0, 8.0, 128, 0.0),
                           max_iter(n)
    {
        set_title("Moire pattern & approximation to Mandelbrot set");
        bmp_name = "mandelmoire01.bmp";

        /* The tweaked plot function makes a difference only
         * when used with antialiasing. 2*2 subsamples seems to
         * look the best.
         */
        run(2);
    }
};

int main(int argc, char **argv) {
    int r(15);
    if (argc >= 2) r = strtol(argv[1], 0, 10);

    try {
        mandelmoire02 M(r);
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

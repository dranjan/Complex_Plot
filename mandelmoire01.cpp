#include "complex_plot.hpp"
#include "angle_tools.hpp"
#include <cstdlib>
#include <algorithm>

using namespace std;

class mandelmoire01 : public complex_plot {
protected:
    int max_iter;

    color color_func(cplx z) {
        cplx w(0.0);
        int k;

        for (k = 0; k < max_iter; ++k) {
            w = w*w + z;
            if (abs(w) > 1e6) break;
        }
        if (k == max_iter) --k;
        
        double t = repdbl(arg(w), max_iter-k);
        
        return HSVcolor(t, 1.0, 1.0);
    }

    /* We'll override the plot function to make pixels with very
     * disparate subsample values darker. This produces a i
     * really cool effect when used with an otherwise bright
     * colormap.
     */
    virtual int plot() {
        assert( screen_init );

        percent_counter pc(screen->h * screen->w);
        if (verbosity > 0) pc.start();

        for (int y = 0; y < screen->h; ++y) {
            for (int x = 0; x < screen->w; ++x) {
                if (!working) return 1;

                color cval = RGBcolor(0, 0, 0);

                for (int k = 0; k < sres; ++k) {
                    for (int j = 0; j < sres; ++j) {
                        double sx = (k+0.5)/sres;
                        double sy = (j+0.5)/sres;

                        cval += color_func(to_plane(x+sx, y+sy));
                    }
                }

                cval /= sres * sres;

                /* Here's the change: basically, set
                 * the value to the saturation.
                 */
                double b = min(min(cval.r, cval.g), cval.b);
                cval.r -= b;
                cval.g -= b;
                cval.b -= b;

                set_pixel(x, y, cval.r, cval.g, cval.b);

                if (verbosity > 0) pc.count();
            }
        }

        working = false;

        update();

        return 0;
    }

public:
    mandelmoire01(int n) : complex_plot(8.0, 8.0, 128, 0.0),
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
        mandelmoire01 M(r);
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

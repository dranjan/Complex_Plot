#ifndef COMPLEX_PLOT_VAR01_HPP
#define COMPLEX_PLOT_VAR01_HPP

#include "complex_plot.hpp"

#include <algorithm>

class complex_plot_var01: public complex_plot {   
protected:
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
    complex_plot_var01(double a, double b, double r, cplx c) :
        complex_plot(a, b, r, c)
    { }
};

//public:

#endif

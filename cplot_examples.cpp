#include <cmath>
#include <iostream>
#include <cstdlib>

#include "complex_plot.hpp"

#ifndef EXAMPLE
#define EXAMPLE example_02
#endif

using namespace std;

/* Example 0: appropriately, draws nothing.
 * This can be used to get a sense of the baseline
 * overhead for making a plot. 
 */
class example_00 : public complex_plot {
protected:
    color colormap(cplx z) {
        return RGBcolor(0, 0, 0);
    }

public:
    example_00() : complex_plot(1, 1, 1024)
    {
        set_title("Example 0");
        run(1);
    }
};

/* Example 1: basic plot of the reciprocal function */
class example_01 : public complex_plot {
protected:
    cplx transform(cplx z) {
        return 1.0/z;
    }

public:
    example_01() : complex_plot(5, 5, 1024/5.0, 2)
    {
        set_title("Example 1");
        run(2);
    }
};

/* Example 2: changing the colormap as well */
class example_02 : public complex_plot {
protected:
    cplx c;

    color colormap(cplx z) {
        return checkermap(z);
    }

    /* This function actually has a branch cut, but due to
     * our colormap, the result looks seamless.
     */
    cplx transform(cplx z) {
        return log((z-1.0)/(z+1.0))*c;
    }

public:
    /* a, b determine the shape of the double-spiral */
    example_02(int a, int b) : c(a/M_PI, b/M_PI) { }

    example_02() : complex_plot(5.0, 5.0, 1024/5.0), 
                   c(8/M_PI, 13/M_PI)
    {
        set_title("Example 2");
        bmp_name = "bispiral01.bmp";
        run(16);
    }
};


/* Example 3: changing the color function entirely */
class example_03 : public complex_plot {
protected:
    double bailout;

    color color_func(cplx z) {
        double a = bailout;
        unsigned int n = 0;

        while (abs(imag(z)) > a) {
            z = sin(z);
            a = sinh(a);
            ++n;
        }

        return RGBcolor(1.0/(1+n*n), 2.0/(2+n*n), 3.0/(3+n*n));
    }

public:
    /* Guideline: the window height should be at least 2*b */
    example_03(double b) : bailout(b) { }

    example_03() : bailout(1)
    {
        height_set_window(1024, 1024, 10);

        set_title("Example 3");
        bmp_name = "sine_lobes01.bmp";
        run(2);
    }
};

/* Example 4: an iterated rational function that can be 
 * used to approximate an elliptic function
 */
class example_04 : public complex_plot {
protected:
    int n_;

    cplx f(cplx z) {
        z *= z;
        return cplx(-z.real(), 1-z.imag()) /
               cplx( z.real(), 1+z.imag());
    }

    cplx transform(cplx z) {
        for (int k = 0; k < n_; ++k) {
            z = f(z);
        }
        return z;
    }
public:
    example_04(int n) : n_(n) { }

    example_04() : n_(10) {
        set_title("Example 4");
        bmp_name = "ell_rat01.bmp";
        width_set_window(1024, 1024, 4);
        run(2);
    }
};

int main(int argc, char **argv) {
    try { 
        EXAMPLE();
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

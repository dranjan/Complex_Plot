#include "complex_plot.hpp"

using namespace std;

class mandel : public complex_plot {
protected:
    int max_iter;
    double ptol;
    double bailout;
    double logb;

    cplx f(cplx c, cplx z) {
        return z*z + c;
    }

    double count(cplx c) {
        cplx z = 0.0;
        cplx z2 = z;

        for (int n = 0; n < max_iter; ++n) {
            z = f(c, z);
            z2 = f(c, f(c, z2));

            double d = abs(z2);
            if (d > bailout) {
                /* Escaped */
                return 2*n - log2(log(d)/logb);
            }

            if (abs(z2-z) < ptol) {
                /* Looks like we hit a period */
                return -HUGE_VAL;
            }
        }

        return HUGE_VAL;
    }

    color color_func(cplx c) {
        double d = count(c);
        if (d == HUGE_VAL) {
            return RGBcolor(0.0, 0.0, 0.0);
        } else {
            return colormap(exp(cplx(0.05, 0.05)*d));
        }
    }

    void init(int m, double b, double tol) {
        max_iter = m;
        bailout = b;
        logb = log(b);
        ptol = tol;
    }

public:
    mandel(int m, double b, double tol) {
        init(m, b, tol);
    }

    mandel(bool b = true) {
        if (b) {
            init(300, 1e60, 1e-10);
            set_window(4, 3, 300, -0.5);
            set_title("Smooth Mandelbrot");
            bmp_name = "mandel01.bmp";
            run(2);
        }
    }
};

int main(int argc, char **argv) {
    try { 
        mandel();
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

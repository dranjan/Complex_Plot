#include "complex_plot.hpp"

cplx sine_map(cplx c, cplx z) {
    return sin(z)/c;
}

class mandel_sine : public complex_plot {
protected:
    int max_iter;
    double ptol;
    double bailout;

    double count(cplx s) {
        if (abs(s) > 1) return HUGE_VAL;

        cplx c = sqrt(s);
        cplx z = M_PI/2;
        cplx z2 = z;

        for (int n = 0; n < max_iter; ++n) {
            z = sine_map(c, z);
            z2 = sine_map(c, sine_map(c, z2));

            double d = abs(z.imag());
            if (d > bailout) {
                return n;
            }

            if (abs(z2 - z) < ptol) {
                return -HUGE_VAL;   
            }
        }

        return HUGE_VAL;
    }

    color color_func(cplx s) {
        double d = count(s);
        if (d == HUGE_VAL) {
            return RGBcolor(0.0, 0.0, 0.0);
        } else {
            return colormap(exp(cplx(0.04, 0.04)*d));
        }
    }

    void init(int m, double b, double tol) {
        max_iter = m;
        bailout = b;
        ptol = tol;
    }

public:
    mandel_sine(int m, double b, double tol) {
        init(m, b, tol);
    }

    mandel_sine(bool b = true) {
        if (b) {
            init(200, 10, 1e-8);
            set_window(2, 2, 800);
            set_title("Sine Mandelbrot");
            bmp_name = "mandel_sine01.bmp";
            run(3);
        }
    }
};

int main(int argc, char **argv) {
    try { 
        mandel_sine();
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

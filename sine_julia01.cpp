#include "complex_plot.hpp"

cplx sine_map(cplx c, cplx z) {
    return sin(z)/c;
}

class sine_julia : public complex_plot {
protected:
    int max_iter;
    double ptol;
    double bailout;

    double count(cplx z) {
        cplx z2 = z;
        cplx w = 0.0;

        static const cplx c(1,0);

        for (int n = 0; n < max_iter; ++n) {
            w = z;
            z = sine_map(c, z);
            z2 = sine_map(c, sine_map(c, z2));

            double d = abs(z.imag());
            if (d > bailout) {
                return w.real();
            }

            if (abs(z2 - z) < ptol) {
                return NAN;   
            }

            /* Experimentally, it appears that these circles are
             * attracted to the fixed point at the origin.  This extra
             * clause results in a pretty drastic speed improvement in
             * practice.
             */
            if (abs(z - M_PI/2) < M_PI/2 ||
                abs(z + M_PI/2) < M_PI/2)
            {
                return NAN;
            }
        }

        return NAN;
    }

    color color_func(cplx s) {
        double d = count(s);
        if (isnan(d)) {
            return RGBcolor(0.0, 0.0, 0.0);
        } else {
            d = floor(d/M_PI + 0.5);
            return HSVcolor(1.5 + 0.1016*d*2*M_PI, 1, 1);
        }
    }

    void init(int m, double b, double tol) {
        max_iter = m;
        bailout = b;
        ptol = tol;
    }

public:
    sine_julia(int m, double b, double tol) {
        init(m, b, tol);
    }

    sine_julia(bool b = true) {
        if (b) {
            init(200, 1e2, 1e-4);
            set_window(30, 20, 80);
            set_title("Sine Julia fractal");
            bmp_name = "sine_julia01.bmp";
            run(3);
        }
    }
};

int main(int argc, char **argv) {
    try { 
        sine_julia();
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

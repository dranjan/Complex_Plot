#include "complex_plot.hpp"

#include <vector>

cplx sine_map(cplx c, cplx z) {
    return sin(z)/c;
}

class sine_julia : public complex_plot {
protected:
    int max_iter;
    double ptol;
    double bailout;
    double bailout2;

    cplx c;

    double count(cplx z) {
        cplx z2 = z;

        for (int n = 0; n < max_iter; ++n) {
            double d = abs(z.imag());

            if (d > bailout) {
                if (d > 1e2) return n;
                double d2 = abs(imag(sine_map(c, z)));
                if (!isnan(d2) and d2 > bailout2) return n;
                else return NAN; //n + 3.5;
            }

            z = sine_map(c, z);
            z2 = sine_map(c, sine_map(c, z2));

            if (abs(z2 - z) < ptol) {
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
            //d = floor(d/M_PI + 0.5);
            return HSVcolor(1.5 + d*2*M_PI*0.2, 1, 1);
        }
    }

    void init(int m, double b, double tol, cplx c) {
        max_iter = m;
        bailout = b;
        ptol = tol;
        this->c = c;

        bailout2 = cosh(bailout)/abs(c);
    }

public:
    sine_julia(int m, double b, double tol, cplx c) {
        init(m, b, tol, c);
    }

    sine_julia(bool b = true) {
        if (b) {
            //init(200, 5, 1e-8, cplx(1,1)*sqrt(0.5));
            init(200, 10, 1e-8, cplx(0.5, 0.3));
            set_window(30, 20, 80);
            set_title("Sine Julia fractal");
            bmp_name = "sine_julia02.bmp";
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

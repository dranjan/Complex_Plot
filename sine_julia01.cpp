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

    int min_i;
    int max_i;

    double count(cplx z) {
        cplx z2 = z;

        static const cplx c(1,0);

        std::vector<cplx> trajectory;
        trajectory.reserve(max_i - min_i);

        for (int n = 0; n < max_iter; ++n) {
            double d = abs(z.imag());

            if (min_i <= n && n < max_i) {
                trajectory.push_back(z);
            }

            if (d > bailout) {
                //return n;

                //std::cout << d << std::endl;

                if (trajectory.size() < 1) return NAN;

                double a = trajectory.back().real();
                //if (trajectory.back().imag() < 0) a = -a;

                for (unsigned int k = trajectory.size()-1;
                     k >= 1; --k)
                {
                    a = atan(a/1e2)/M_PI;
                    if (trajectory[k].imag() < 0) a = -a;
                    a += dtoi(trajectory[k-1].real()/M_PI);
                }

                return a;

                //double d = atan(z.real()/1e2)/M_PI;
                //if (imag(z) < 0) d = -d;
                //return dtoi(w.real()/M_PI) + d;
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
            return HSVcolor(d*2*M_PI, 1, 1);
        }
    }

    void init(int m, double b, double tol) {
        max_iter = m;
        bailout = b;
        ptol = tol;

        min_i = 0;
        max_i = 4;
    }

public:
    sine_julia(int m, double b, double tol) {
        init(m, b, tol);
    }

    sine_julia(bool b = true) {
        if (b) {
            init(200, 1e2, 1e-8);
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

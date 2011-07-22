#include "complex_plot.hpp"
#include "angle_tools.hpp"

using namespace std;

class mandel_ca : public complex_plot {
protected:
    int max_iter;
    double ptol;
    double bailout;
    double logb;
    double theta;

    cplx f(cplx c, cplx z) {
        return z*z + c;
    }

    cplx pre_arg(cplx c) {
        cplx z = 0.0;
        cplx z2 = z;

        for (int n = 0; n < max_iter; ++n) {
            z = f(c, z);
            z2 = f(c, f(c, z2));

            double d = abs(z);
            if (d > bailout) {
                /* Escaped */
                /* One more iteration for accuracy's sake */
                double u = arg(z);
                double v = div_angle(arg(f(c, f(c, z))), 4, u);

                double alpha = fix_angle(v - repdbl(theta, n));
                return cplx(n, alpha);
            }

            if (abs(z2-z) < ptol) {
                /* Looks like we hit a period */
                return cplx(NAN, NAN);
            }
        }

        return cplx(NAN, NAN);
    }

    color color_func(cplx c) {
        cplx d = pre_arg(c);
        if (isnan(d.imag())) {
            return RGBcolor(1.0, 1.0, 1.0);
        } else {
            double r = 1-1.0/sqrt(log2(d.real()+1.0));
            if (d.imag() > 0) return RGBcolor(r, r, r);
            else return RGBcolor(1-0.5*(1-r), r, 1);
        }
    }

    void init(int m, double b, double tol, double th) {
        max_iter = m;
        bailout = b;
        logb = log(b);
        ptol = tol;
        theta = th;
    }

public:
    mandel_ca(int m, double b, double tol, double th) {
        init(m, b, tol, th);
    }

    mandel_ca(bool b = true) {
        if (b) {
            init(300, 10, 1e-10, 2*M_PI*0.618033989);
            set_window(4, 3, 300, -0.5);
            set_title("Banded Mandelbrot");
            bmp_name = "mandel_ca01.bmp";
            run(3);
        }
    }
};

int main(int argc, char **argv) {
    try { 
        mandel_ca();
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

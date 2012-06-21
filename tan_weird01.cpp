#include "complex_plot.hpp"

using namespace std;

class tan_weird01: public complex_plot {
protected:

    cplx lambda;
    double tol;
    int max_iter;

    color color_func(cplx z) {
        int n = 0;
        //double r = HUGE_VAL;
        //double f = sqrt(0.5);
        color ret = RGBcolor(0,0,0);

        while (n < max_iter) {

            cplx test(mod(z.real() + M_PI/2, M_PI) - M_PI/2, abs(z.imag()));

            //r = abs(step);
            //if (abs(test.real()) <= tol && abs(test.imag()) <= 0.5) {
            if (abs(test - lambda) <= tol) {
                ret = HSVcolor(1.5 + (sqrt(5)-1)/2*z.real(), 1, 1);
                break;
            } else {
                cplx step = -tan(z);
                z += 2.0*step;
                //z += 2.0*step;
                ++n;
            }
        }

        //return var_colormap(sin(z));

        //ret = HSLcolor(1.5 + (sqrt(5)-1)/2*z.real(), 1, 1.0/(1.0+abs(z.imag())));
        //return HSVcolor(1.5 + (sqrt(5)-1)/2*z.real() + 0.1*(2*(z.imag() >= 0)-1), 1, 
        //        1.0/(1.0+abs(z.imag())));

        if (n < max_iter) {
            double count = n; // - log(log(f*r)/log(f*tol))/log(3);
            return ret/sqrt(1+count/4);
        } else {
            return ret;
        }
    }

    void init(cplx c, int m, double t) {
        max_iter = m;
        tol = t;
        lambda = cplx(0, tol);
    }

public:
    tan_weird01(bool b = true) {
        if (b) {
            cplx c(0.0);

            init(c, 60, 0.1);

            width_set_window(1024, 1024, 5*M_PI, cplx(0, 0));

            bmp_name = "tan_weird01.bmp";
            set_title("Newton's method (basins of attraction)");

            run(1);
        }
    }
};

int main(int argc, char **argv) {
    try { 
        tan_weird01();
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}


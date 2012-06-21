#include "complex_plot.hpp"

using namespace std;

class newton02 : public complex_plot {
protected:

    double tol;
    int max_iter;

    color color_func(cplx z) {
        int n = 0;
        double r = HUGE_VAL;
        double f = sqrt(0.5);
        color ret = RGBcolor(0,0,0);

        while (n < max_iter) {
            cplx step = -tan(z);
            r = abs(step);
            if (r < tol) {
                ret = HSVcolor(1.5 + (sqrt(5)-1)/2*z.real(), 1, 1);
                break;
            } else {
                z += step;
                ++n;
            }
        }
        
        if (n < max_iter) {
            double count = n - log(log(f*r)/log(f*tol))/log(3);
            return ret/(1+count/4);
        } else {
            return ret;
        }
    }

    void init(int m, double t) {
        max_iter = m;
        tol = t;
    }

public:
    newton02(bool b = true) {
        if (b) {
            init(300, 1e-3);

            width_set_window(1024, 1024, 1, cplx(M_PI/2));

            bmp_name = "newton02.bmp";
            set_title("Newton's method (basins of attraction)");

            run(1);
        }
    }
};

int main(int argc, char **argv) {
    try { 
        newton02();
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}


#include "complex_plot.hpp"

class mandel_iter04 : public complex_plot {
protected:
    int n;

    cplx transform(cplx c) {
        cplx z(c);
        cplx w(0);
        int k = 0;
        while (k < n-1) {
            ++k;
            w = w*w + c; //1.0 + c*(w*w);
            if (abs(w) > 1e80) break;
            z *= pow(1.0 + c/(w*w), pow(0.5, k));
        }
        return z;
    }

    color colormap(cplx z) {
        double n(std::norm(z));
        //if (n < 1) n = HUGE_VAL;
    
        n = std::sqrt(n);
        //n = std::log(n);
        n = (n-1)/(n+1);
        double d(std::acos(2*sqrt(abs(n)) - 1)/M_PI);
        //double d(std::acos(n)/M_PI);

        return HSLcolor(std::arg(z), 1.0, d);
    }

    void init(int num_iter) {
        n = num_iter;
    }

public:
    mandel_iter04(int n = 1) {
        if (n >= 0) {
            init(n);
            width_set_window(1600, 1200, 6, -0.5);
            set_title("Mandelbrot iterate");
            bmp_name = "mandel_iter01.bmp";
            run(3);
        }
    }
};

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " num_iter" << endl;
        return 2;
    }

    try {
        int n = atoi(argv[1]);
        (mandel_iter04(n));
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

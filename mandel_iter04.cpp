#include "complex_plot.hpp"

class mandel_iter04 : public complex_plot {
protected:
    int n;
    int m;

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

        for (int j = 0; j < m; ++j) {
            if (abs(z) > 1e80) break;
            z *= z;
        }
   
        return z*pow(0.5, pow(0.5, n-1-m));
    }

    color colormap(cplx z) {
        double n(std::norm(z));
        if (n < 1) n = pow(n, pow(2.0, this->n-1-m));
        //if (n < 1) n = HUGE_VAL;
    
        n = std::sqrt(n);
        //n = std::log(n);
        n = (n-1)/(n+1);
        double d(std::acos(2*sqrt(abs(n)) - 1)/M_PI);
        //double d(std::acos(n)/M_PI);
        if (std::norm(z) < 1) d = 1 - d;

        return HSLcolor(std::arg(z), 1.0, d);
    }

    void init(int num_iter, int num_post_iter) {
        n = num_iter;
        m = num_post_iter;
    }

public:
    mandel_iter04(int n = 1, int m = 0) {
        if (n >= 0) {
            init(n, m);
            width_set_window(1600, 1200, 6, -0.5);
            set_title("Mandelbrot iterate");
            bmp_name = "mandel_iter01.bmp";
            run(3);
        }
    }
};

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0]
             << " num_iter [num_post_iter]" << endl;
        return 2;
    }

    try {
        int n = atoi(argv[1]);
        int m = 0;
        if (argc >= 3) {
            m = atoi(argv[2]);
        }
        (mandel_iter04(n, m));
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

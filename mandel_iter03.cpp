#include "complex_plot.hpp"

class mandel_iter03 : public complex_plot {
protected:
    int n;

    cplx f(cplx c, cplx z) {
        return z*z + c;
    }

    cplx transform(cplx c) {
        cplx z(0.0);
        cplx w(0.0);
        int k;
        for (k = 0; k < n; ++k) {
            w = 2.0*z*w + 1.0;
            z = f(c, z);
            if (abs(z) >= 1e10) {
                ++k;
                break;
            }
        }
        return z/w*pow(2.0,k);
    }

    void init(int num_iter) {
        n = num_iter;
    }

public:
    mandel_iter03(int n = 1) {
        if (n >= 0) {
            init(n);
            width_set_window(1200, 900, 4, -0.5);
            set_title("Mandelbrot iterate");
            bmp_name = "mandel_iter03.bmp";
            run(2);
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
        (mandel_iter03(n));
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

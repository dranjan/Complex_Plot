#include "complex_plot.hpp"

class mandel_iter02 : public complex_plot {
protected:
    int n;

    cplx f(cplx c, cplx z) {
        return z*z + c;
    }

    cplx transform(cplx c) {
        cplx z(0.0);
        cplx t(0.0);
        for (int k = 0; k < n; ++ k) {
            z = f(c, z);
            t += 1.0/z;
            if (abs(z) >= 1e10) {
                break;
            }
        }
        return 1.0/t;
    }

    void init(int num_iter) {
        n = num_iter;
    }

public:
    mandel_iter02(int n = 1) {
        if (n >= 0) {
            init(n);
            width_set_window(1200, 900, 4, -0.5);
            set_title("Mandelbrot iterate");
            bmp_name = "mandel_iter01.bmp";
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
        (mandel_iter02(n));
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

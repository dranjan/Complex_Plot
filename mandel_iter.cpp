#include "complex_plot.hpp"

class mandel_iter : public complex_plot {
protected:
    int n;

    cplx f(cplx c, cplx z) {
        return z*z + c;
    }

    cplx transform(cplx c) {
        cplx z(0.0);
        for (int k = 0; k < n; ++ k) {
            z = f(c, z);
            if (abs(z) >= 1e100) {
                return cplx(1e100);
            }
        }
        return z;
    }

    void init(int num_iter) {
        n = num_iter;
    }

public:
    mandel_iter(int n = 1) {
        if (n >= 0) {
            init(n);
            set_window(4, 3, 300, -0.5);
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
        (mandel_iter(n));
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

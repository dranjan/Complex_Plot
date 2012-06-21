#include <cmath>
#include <iostream>
#include <cstdlib>
#include <string>

#include "complex_plot.hpp"
#include "elliptic_function01.hpp"

using namespace std;

class elliptic_test01 : public complex_plot {
protected:
    elliptic_function E;

    double f;

    cplx transform(cplx z) {
        return E.compute_raw(z)*f;
    }

public:
    elliptic_test01() : E(exp(cplx(0, 0.5*M_PI/3)),
            exp(cplx(0, -0.5*M_PI/3)))  
    {
        E.n_ = 4;
        f = 1.0/abs(E.a_);

        set_title("Elliptic function approximation");
        bmp_name = "ell_rat02.bmp";
        width_set_window(1024, 1024, 16);

        run(2);
    }
};

class elliptic_test02 : public complex_plot {
protected:
    elliptic_function E;

    double f;

    cplx transform(cplx z) {
        return E.compute_raw(z)*f;
    }

public:
    elliptic_test02() : E(exp(cplx(0, 0.0*M_PI/3)),
            3.0*exp(cplx(0, 0.5*M_PI/3)))  
    {
        E.n_ = 6;
        f = 1.0/abs(E.a_);

        set_title("Elliptic function approximation");
        bmp_name = "ell_rat03.bmp";
        width_set_window(1024, 1024, 64);

        run(2);
    }
};

int main(int argc, char **argv) {
    try { 
        elliptic_test01 E;
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

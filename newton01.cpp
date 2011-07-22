#include "complex_plot.hpp"

using namespace std;

cplx ni(cplx c, cplx z) {
    return z - 1.0/(1.0/(z-1.0) + 
                    1.0/(z+0.5+c) +
                    1.0/(z+0.5-c));
}

color ni_color_func(cplx lambda, cplx z, int max_iter, double tol) {
    int n = 0;
    double r = HUGE_VAL;
    double f = 3;
    color ret = RGBcolor(0,0,0);

    //cplx z2 = z;

    while (n < max_iter) {
        if (abs(z-1.0) < tol) {
            r = abs(z-1.0);
            f = 3;
            ret = RGBcolor(1,0,0);
            break;
        } else if (abs(z+0.5+lambda) < tol) {
            r = abs(z+0.5+lambda);
            f = 3*abs(-0.5-lambda);
            ret = RGBcolor(0,1,0);
            break;
        } else if (abs(z+0.5-lambda) < tol) {
            r = abs(z+0.5-lambda);
            f = 3*abs(-0.5+lambda);
            ret = RGBcolor(0,0,1);
            break;
        //} else if (abs(z2-z) < tol) {
        //    n = max_iter;
        //    break;
        } else {
            z = ni(lambda, z);
            //z2 = ni(lambda, ni(lambda, z2));
            ++n;
        }
    }

    if (n < max_iter) {
        double count = n - log(log(f*r)/log(f*tol))/log(2);
        return ret/sqrt(1+count);
    } else {
        return ret;
    }
}


/* From Douady-Hubbard paper on "polynomial-like mappings */
class newton01 : public complex_plot {
protected:

    cplx lambda;
    double tol;
    int max_iter;

    color color_func(cplx z) {
        return ni_color_func(lambda, z, max_iter, tol);
    }

    void init(cplx c, int m, double t) {
        lambda = c;
        max_iter = m;
        tol = t;
    }

public:
    newton01(bool b = true) {
        if (b) {
            cplx c(-0.0100605, 0.220311);
            //cplx c(0, sqrt(3)/2);

            init(c, 300, 1e-7);

            width_set_window(1024, 1024, 10.0);

            bmp_name = "newton01.bmp";
            set_title("Newton's method (basins of attraction)");

            run(2);
        }
    }
};


class newton01_mand : public complex_plot {
protected:
    double tol;
    int max_iter;

    color color_func(cplx z) {
        return ni_color_func(z, 0.0, max_iter, tol);
    }

    void init(int m, double t) {
        max_iter = m;
        tol = t;
    }

public:
    newton01_mand(bool b = true) {
        if (b) {
            init(300, 1e-7);

            width_set_window(1024, 1024, 0.01, 
                             cplx(-0.0100605, 0.220311));

            bmp_name = "newton01_mand.bmp";
            set_title("Newton's method (Mandelbrot)");

            run(1);
        }
    }
};

int main(int argc, char **argv) {
    try { 
        newton01_mand();
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}


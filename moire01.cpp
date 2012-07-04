#include "complex_plot.hpp"
#include <cstdlib>

using namespace std;

class moire01 : public complex_plot {
protected:
    double d;

    color color_func(cplx z) {
        //double x = (int(floor(norm(z))) % 6)/6.0*2*M_PI;
        return HSVcolor(norm(z)*d, 1.0, 1.0);
        //return checkermap(sqrt(sqrt(z)));
    }

public:
    //moire01(double a) : d(a) { }
    moire01(double a) : complex_plot(2.0, 2.0, 512, 0.0),
                        d(a)
    {
        set_title("Moire pattern");
        bmp_name = "moire01.bmp";
        run(1);
    }
};

int main(int argc, char **argv) {
    double r = 2000;
    if (argc >= 2) r = strtod(argv[1], 0);

    try {
        moire01 M(r);
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

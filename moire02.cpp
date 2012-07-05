#include "complex_plot_var01.hpp"
#include <cstdlib>

using namespace std;

class moire02 : public complex_plot_var01 {
protected:
    double d;

    color color_func(cplx z) {
        //double x = (int(floor(norm(z))) % 6)/6.0*2*M_PI;
        return HSVcolor(norm(z)*d, 1.0, 1.0);
        //return checkermap(sqrt(sqrt(z)));
    }

public:
    //moire01(double a) : d(a) { }
    moire02(double a) : complex_plot_var01(2.0, 2.0, 512, 0.0),
                        d(a)
    {
        set_title("Moire pattern");
        bmp_name = "moire02.bmp";
        run(2);
    }
};

int main(int argc, char **argv) {
    double r = 4000;
    if (argc >= 2) r = strtod(argv[1], 0);

    try {
        moire02 M(r);
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

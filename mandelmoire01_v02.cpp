#include <cstdlib>

#include "mandelmoire01.hpp"

class mandelmoire01_v02 : public mandelmoire01 {
public:
    mandelmoire01_v02(int n = 32) : mandelmoire01(n)
    {
        set_window(8.0, 8.0, 128.0, 0.0);
        recenter(1.0/2.2406e-07,
                 cplx(-0.1433584988726666, 1.0525490351244));
        //recenter(0.00001/5.44467e-10, cplx(-0.159793, 1.0419));

        set_title("mandelmoire01_v02");
        bmp_name = "mandelmoire01_v02.bmp";
        run(2);
    }
};

using namespace std;

int main(int argc, char **argv) {
    int r(32);
    if (argc >= 2) r = strtol(argv[1], 0, 10);

    try {
        mandelmoire01_v02 M(r);
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

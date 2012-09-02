#include <cstdlib>

#include "mandelmoire01.hpp"

class mandelmoire01_v01 : public mandelmoire01 {
public:
    mandelmoire01_v01(int n) : mandelmoire01(n)
    {
        set_window(8.0, 8.0, 128.0, 0.0);
        set_title("mandelmoire01_v01");
        bmp_name = "mandelmoire01_v01.bmp";
        run(2);
    }
};

using namespace std;

int main(int argc, char **argv) {
    int r(15);
    if (argc >= 2) r = strtol(argv[1], 0, 10);

    try {
        mandelmoire01_v01 M(r);
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

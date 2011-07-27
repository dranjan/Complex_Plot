#include <math.h>

#define S_ITER 48

double s_high(double a) {
    double t = 0;
    double b = 0;
    double p = 1;
    int k;

    a -= floor(a);

    for (k = 0; k < S_ITER; ++k) {
        b += a;
        p /= 2;
        if (b >= 1) b -= 1;
        if (b + a >= 1) t += p;
    }

    return t;
}

double s_low(double a) {
    return 1-s_high(1-a);
}

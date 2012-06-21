#include <cmath>
#include <iostream>
#include <cstdlib>
#include <string>
#include <cassert>

#include "complex_plot.hpp"

cplx theta_function(cplx tau, cplx z) {
    assert( tau.imag() > 0 );

    cplx q(std::exp(cplx(0, M_PI)*tau));
    cplx q2(q*q);
    cplx w(std::exp(cplx(0, 2*M_PI)*z));

    cplx t(1.0);
    
    cplx qn = q;
    cplx wn = w;
    cplx f = q;
    cplx term;

    do {
        term = qn*(wn + 1.0/wn);
        t += term;
        f *= q2;
        qn *= f;
        wn *= w;
    } while (std::abs(term) > 1e-10);

    return t;
}

cplx theta_function(unsigned int i, unsigned int j, cplx tau, cplx z) {
    z += double(j)*0.5;
    cplx f(exp(double(i)*cplx(0, M_PI)*(tau*0.25 + z)));
    z += double(i)*0.5*tau;

    return theta_function(tau, z)*f;
}

class theta_plot : public complex_plot {
protected:
    cplx tau_;
    int i_, j_;
    cplx a_, b_;

    cplx weierstrass(cplx z) {
        z = theta_function(0,1, tau_, z)/theta_function(1,1, tau_, z);
        return a_*z*z + b_;
    }

    cplx transform(cplx z) {
        return theta_function(0,0, tau_, z)/theta_function(0,1, tau_, z); 
        //weierstrass(z); //*z;
        //return theta_function(i_, j_, tau_, z);
    }
    
    color colormap_var(cplx z) {
        double n(std::norm(z));

        n = std::log(1 + std::sqrt(n));
        n = (n-1)/(n+1);
        double d(std::acos(n)/M_PI);

        return HSLcolor(std::arg(z), 1.0, d);
    }

    void init(cplx tau, int i, int j) {
        tau_ = tau;
        i_ = i;
        j_ = j;
        
        a_ = M_PI*theta_function(tau_, 0.0)*theta_function(1,0, tau_, 0.0);
        a_ *= a_;

        cplx t = theta_function(tau_, 0.0);
        t *= t;
        t *= t;

        cplx s = theta_function(1,0, tau_, 0.0);
        s *= s;
        s *= s;

        b_ = -(M_PI*M_PI/3)*(s + t);
    }

public:
    theta_plot() : tau_(0.0, 1.0), i_(1), j_(1) {
        init(exp(cplx(0.0, M_PI/2)), 1, 1);

        width_set_window(1024, 1024, 10.0);
        run(1);
    }
};

int main(int argc, char **argv) {
    try { 
        theta_plot E;
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

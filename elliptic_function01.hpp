#ifndef ELLIPTIC_FUNCTION01_HPP
#define ELLIPTIC_FUNCTION01_HPP

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <string>

#include "complex_plot.hpp"

class elliptic_function {
public:
    int n_;
    cplx a_, b_, c_;
    cplx f_, f2_;

protected:
    cplx tau_;

    void process_periods(cplx tau1, cplx tau2) {
        if (tau1 == cplx(0.0)) {
            throw std::string("period cannot be zero");
        }

        cplx tau(tau2/tau1);
        cplx f(1.0/tau1);

        if (tau.imag() == 0) {
            throw std::string("degenerate periods");
        }

        if (tau.imag() < 0) {
            tau = 1.0/tau;
            f = 1.0/tau2;
        }

        tau.real() -= floor(tau.real() + 0.5);
        while (norm(tau) < 1.0) {
            tau = -1.0/tau;
            tau.real() -= floor(tau.real() + 0.5);
            f *= tau;
        }

        f_ = f;
        f2_ = f*f;
        tau_ = tau;
    }

    void init_from_periods(int n, cplx tau1, cplx tau2) {
        process_periods(tau1, tau2);

        cplx q(std::exp(tau_*cplx(0, M_PI)));
        cplx q2(q*q);

        /* e1 */

        cplx e1(0.0);
        for (cplx q2k(q2); ; q2k *= q2) {
            cplx t1(1.0 + q2k);
            cplx t2(1.0 - q2k);
            cplx term(q2k*(1.0/(t1*t1) + 1.0/(t2*t2)));

            if (std::abs(term) <= 1e-10) break;

            e1 += term;
        }

        e1 = (e1*8.0 + 2.0/3)*M_PI*M_PI;

        /* e2 */

        cplx e2(0.0);
        cplx qk;
        double s = -1;
        for (qk = q, s = -1; ; qk *= q, s = -s) {
            cplx t1(1.0 - qk);
            cplx term(s*qk/(t1*t1));

            if (std::abs(term) <= 1e-10) break;
        
            e2 += term;
        }

        e2 = (e2*8.0 - 1.0/3)*M_PI*M_PI;

        /* e3 */

        cplx e3(0.0);
        for (qk = q, s = -1; ; qk *= q, s = -s) {
            cplx t1(1.0 - s*qk);
            cplx term(qk/(t1*t1));

            if (std::abs(term) <= 1e-10) break;
        
            e3 += term;
        }
        
        e3 = (e3*8.0 - 1.0/3)*M_PI*M_PI;

        init(n, e1, e2, e3);
    }

    cplx R(cplx z) {
        cplx w((z - b_)/(z - 1.0));
        cplx t(1.0/b_);
        return 4.0/(t*(z + w) + 1.0/z + 1.0/w);
    }

    cplx transform(cplx z) {
        z /= std::pow(2,n_);
        z *= z*c_;
        for (int k = 0; k < n_; ++k) {
            cplx z1 = R(z);
            z = z1;
        }
        return a_ + c_/z;
    }

    void init(int n, cplx e1, cplx e2, cplx e3) {
        n_ = n;

        a_ = e3;
        c_ = e1 - e3;
        b_ = c_/(e2 - e3);
    }
public:
    elliptic_function(cplx tau) {
        init_from_periods(10, cplx(1.0), tau);
    }

    elliptic_function(cplx tau1, cplx tau2) {
        init_from_periods(10, tau1, tau2);
    }

    cplx compute_raw(cplx z) {
        z *= f_;
        return transform(z)*f2_;
    }

    cplx compute(cplx z) {
        z *= f_;
        z -= std::floor(z.imag()/tau_.imag() + 0.5)*tau_;
        z.real() -= std::floor(z.real() + 0.5);
        return transform(z)*f2_;
    }
};


#endif

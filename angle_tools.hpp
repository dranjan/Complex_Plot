#ifndef ANGLE_TOOLS_HPP
#define ANGLE_TOOLS_HPP

#include "complex_plot.hpp"

/* Compute theta mod 2*pi (but in the range [-pi, pi)) */
inline double proper_angle(double theta) {
    return mod(theta + M_PI, 2*M_PI) - M_PI;
}

/* Simpler function which can be called after a 
 * single addition or subtraction of angles 
 * (or multiplication by 2)
 */
inline double fix_angle(double theta) {
    if (theta > M_PI) return theta - 2*M_PI;
    else if (theta < -M_PI) return theta + 2*M_PI;
    else return theta;
}

/* This will never overflow, unlike theta*pow(2, n) */
inline double repdbl(double theta, int n) {
    for (int k = 0; k < n; ++k) {
        theta = fix_angle(theta*2);
    }
    return theta;
}

/* Returns closest psi to phi such that n*psi = theta (mod 2*pi) */
inline double div_angle(double theta, int n, double phi) {
    int m = dtoi((n*phi - theta)/(2*M_PI));
    return (theta + 2*M_PI*m)/n;
}

#endif

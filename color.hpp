#ifndef COLOR_H
#define COLOR_H

#include <cstdlib>
#include <stdint.h>
#include <cmath>
//#include "constants.hpp"

using namespace std;

struct color {
    double r;
    double g;
    double b;

    color & operator += (const color & c) {
        r += c.r;
        g += c.g;
        b += c.b;

        return *this;
    }

    color & operator *= (double s) {
        r *= s;
        g *= s;
        b *= s;

        return *this;
    }

    color & operator /= (double s) {
        r /= s;
        g /= s;
        b /= s;

        return *this;
    }
};

inline int mymod(int a, int b) {
    return (a >= 0)? a%b : (b-(-a)%b)%b;
}

inline color RGBcolor(double r, double g, double b) {
    color c = {r, g, b};
    //c.r = r; c.g = g; c.b = b;
    return c;
}

inline color operator * (const color & c, double s) {
    return RGBcolor(c.r*s, c.g*s, c.b*s);
}

inline color operator * (double s, const color & c) {
    return RGBcolor(c.r*s, c.g*s, c.b*s);
}

inline color operator / (const color & c, double s) {
    return RGBcolor(c.r/s, c.g/s, c.b/s);
}

inline color hexcolor(uint32_t h) {
    color c;
    c.r = ((h>>16)&0xFF)/255.0;
    c.g = ((h>>8)&0xFF)/255.0;
    c.b = (h&0xFF)/255.0;
    return c;
}

/* These are probably from the internet somewhere, but I don't 
 * recall where.
 */
inline color HSVcolor(double h, double s, double v) {
    color c;
    int Hi = int(floor(h*3/M_PI));
    double f = h*3/M_PI - Hi;
    double p = v*(1-s);
    double q = v*(1-s*f);
    double t = v*(1-s*(1-f));
    switch (mymod(Hi,6)) {
    case 0: c.r = v; c.g = t; c.b = p; break;
    case 1: c.r = q; c.g = v; c.b = p; break;
    case 2: c.r = p; c.g = v; c.b = t; break;
    case 3: c.r = p; c.g = q; c.b = v; break;
    case 4: c.r = t; c.g = p; c.b = v; break;
    case 5: c.r = v; c.g = p; c.b = q; break;
    }
    return c;
}

inline color HSLcolor(double h, double s, double l) {

    h /= (2*M_PI);
    h = h - floor(h);
    l = 1.0 - l;
    double r, g, b;
    float temp1, temp2, tempr, tempg, tempb;
    //If saturation is 0, the color is a shade of gray
    if(s == 0) r = g = b = l;
       //If saturation > 0, more complex calculations are needed
    else
    {
        //Set the temporary values      
        if(l < 0.5) temp2 = l * (1 + s);      
        else temp2 = (l + s) - (l * s);     
        temp1 = 2 * l - temp2;    
        tempr = h + 1.0 / 3.0;    
        if(tempr > 1) tempr--;
        tempg = h;     
        tempb = h - 1.0 / 3.0;
        if(tempb < 0) tempb++; 
        
        //Red     
        if(tempr < 1.0 / 6.0) 
            r = temp1 + (temp2 - temp1) * 6.0 * tempr;      
        else if(tempr < 0.5) 
            r = temp2;   
        else if(tempr < 2.0 / 3.0) 
            r = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempr) * 6.0;
        else 
            r = temp1; 
        
        //Green       
        if(tempg < 1.0 / 6.0) 
            g = temp1 + (temp2 - temp1) * 6.0 * tempg;    
        else if(tempg < 0.5) 
            g = temp2;
        else if(tempg < 2.0 / 3.0) 
            g = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempg) * 6.0;
        else 
            g = temp1; 
        
        //Blue    
        if(tempb < 1.0 / 6.0) 
            b = temp1 + (temp2 - temp1) * 6.0 * tempb;   
        else if(tempb < 0.5) 
            b = temp2; 
        else if(tempb < 2.0 / 3.0) 
            b = temp1 + (temp2 - temp1) * ((2.0 / 3.0) - tempb) * 6.0;    
        else 
            b = temp1;
    }

    return RGBcolor(r, g, b);
}

#endif

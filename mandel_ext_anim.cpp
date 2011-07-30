#include "sdl_anim.hpp"
#include "complex_plot.hpp"
#include "angle_tools.hpp"
#include "percent_counter.hpp"

#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

class mandel_ext_anim : public sdl_animation {
protected:
    /* These define the plotted region 
     * (along with the width and height)
     */
    cplx ul;
    double res;

    /* The subsample resolution */
    int sres;

    int dw;
    int dh;

    int max_iter;
    double ptol;
    double bailout;
    double logb;
    double theta;

    int rays;

    double safety;

    int verbosity;

    vector<cplx> data;

    cplx to_plane_sub(int x, int y) {
        return ul + res/sres*cplx(x, -y);
    }

    pair<int, int> from_plane_sub(cplx z) {
        cplx disp = z - ul;
        int x = (int)(floor(disp.real()*sres/res));
        int y = (int)(floor(-disp.imag()*sres/res));

        return pair<int, int>(x, y);
    }

    cplx & index(int x, int y) {
        return data.at(y*dw + x);
    }

    cplx f(cplx c, cplx z) {
        return z*z + c;
    }

    cplx transform(cplx c) {
        cplx z = 0.0;
        cplx z2 = z;

        cplx r(HUGE_VAL, NAN);

        for (int n = 0; n < max_iter; ++n) {
            z = f(c, z);
            z2 = f(c, f(c, z2));

            double d = abs(z);
            if (d > bailout) {
                /* Escaped */

                /* ceil(r.real()) is the actual iteration count */
                r.real() = n - safety*log2(log(d)/logb);

                double u = arg(z);
                double v = div_angle(arg(f(c, f(c, z))), 4, u);

                if (!isnan(theta)) {
                    r.imag() = fix_angle(v - repdbl(theta, n));
                } else {
                    r.imag() = v;
                }
                break;
            }

            if (abs(z2-z) < ptol) {
                /* Looks like we hit a period */
                break;
            }
        }

        return r;
    }

    void init_samples() {
        percent_counter pc(dw * dh);
        if (verbosity > 0) pc.start();

        for (int y = 0; y < dh; ++y) {
            for (int x = 0; x < dw; ++x) {
                index(x, y) = transform(to_plane_sub(x, y));
                
                if (verbosity > 0) pc.count();
            }
        }
    }

    double fact;

    color colormap(cplx z, double off) {
        if (isnan(z.imag())) {
            return RGBcolor(0.0, 0.0, 0.0);
        }

        if (!isnan(theta)) z.imag() -= theta;

        double alpha = floor(fact*(z.imag()-off))/fact + off;
        return HSLcolor(alpha, 1, z.real());
    }

    void preprocess_data() {
        percent_counter pc(dw * dh);

        if (verbosity > 0) pc.start();

        for (int y = 0; y < dh; ++y) {
            for (int x = 0; x < dw; ++x) {
                cplx z = index(x, y);

                if (isnan(z.imag())) continue;

                int n = ceil(z.real());
                double r = n - z.real();
                r = n - r/safety;
                index(x, y).real() = acos((r-1)/(r+1))/M_PI;

                if (verbosity > 0) pc.count();
            }
        }
    }

    void render_frame(double off) {
        percent_counter pc(screen->w * screen->h);

        int f = frames.size()-1;

        if (verbosity > 0) cout << "0% complete.";

        for (int y = 0; y < screen->h; ++y) {
            for (int x = 0; x < screen->w; ++x) {

                color cval = RGBcolor(0, 0, 0);

                for (int dx = 0; dx < sres; ++dx) {
                    for (int dy = 0; dy < sres; ++dy) {
                        int x1 = sres*x + dx;
                        int y1 = sres*y + dy;

                        cval += colormap(index(x1, y1), off);
                    }
                }

                cval /= sres * sres;
                set_frame_pixel(f, x, y, cval);

                if (verbosity > 0) pc.count();
            }
        }

        update_frame(f);
    }

    class sample_comp {
        const mandel_ext_anim & parent;
    public:
        sample_comp(const mandel_ext_anim & p) : parent(p)
        { }
        bool operator () (int kl, int kr) {
            return parent.data[kl].real() < parent.data[kr].real();
        }
    };

    void fix_angles() {
        int num_samples = dw*dh;

        vector<int> sample_list(num_samples);
        vector<bool> sample_fixed(num_samples, false);

        for (int k = 0; k < num_samples; ++k) {
            sample_list[k] = k;
        }

        sort(sample_list.begin(), sample_list.end(),
             sample_comp(*this));

        percent_counter pc(num_samples);
        if (verbosity > 0) pc.start();

        for (int i = 0; i < num_samples; ++i) {
            int k = sample_list[i];

            int x = k % (dw);
            int y = k / (dw);

            if (data[k].real() == HUGE_VAL) {
                if (verbosity > 0) {
                    cout << "\n(skipping interior points)" << endl;
                }
                break;
            }

            int n = ceil(data[k].real());

            for (int dx = -1; dx < 2; ++dx) {
                for (int dy = -1; dy < 2; ++dy) {
                    int x1 = x + dx;
                    int y1 = y + dy;

                    if (x1 < 0 || x1 >= dw || y1 < 0 || y1 >= dh) {
                        continue;
                    }

                    int j = dw*y1 + x1;
                    
                    if (!sample_fixed.at(j)) continue;

                    double phi = data[j].imag();
                    data[k].imag() = div_angle(data[k].imag(), 
                                               pow(2, n), phi);

                    sample_fixed[k] = true;
                    break;
                }
                if (sample_fixed[k]) break;
            }

            if (!sample_fixed[k]) {
                double phi = arg(to_plane_sub(x, y));
                double psi = div_angle(data[k].imag(),
                                       pow(2, n), phi);
                data[k].imag() = psi;

                sample_fixed[k] = true;
            }

            if (verbosity > 0) pc.count();
        }
    } 

    void init_data(int w, int h) {
        dw = w;
        dh = h;
        data.resize(w*h);
    }

    void compute(int num_rays, double fps, int loop_ms,
                 int subsample_res = 1)
    {
        assert( screen_init );

        int num_frames = fps*loop_ms/1000;
        delay_ms = 1000/fps;

        sres = subsample_res;
        rays = num_rays;

        fact = rays/(2*M_PI);

        init_data(screen->w*sres, screen->h*sres);

        if (verbosity > 0) {
            cout << "Initializing..." << endl;
        }

        init_samples();

        if (verbosity > 0) {
            cout << "Fixing angles..." << endl;
        }

        fix_angles();

        if (verbosity > 0) {
            cout << "Preprocessing for rendering..." << endl;
        }

        preprocess_data();

        for (int k = 0; k < num_frames; ++k) {
            add_frame();
            if (verbosity > 0) {
                cout << "Frame " << (k+1) << "/" << num_frames << endl;
            }
            render_frame(2*M_PI*k/num_frames/rays);
        }

        run();
    }

    void init(int m, double b, double tol, double th) {
        max_iter = m;
        bailout = b;
        logb = log(b);
        ptol = tol;
        theta = th;

        verbosity = 1;

        safety = 0.3;
    }

public:
    void set_window(double real_sz, double imag_sz, double pixel_res,
                    cplx center = 0.0)
    {
        int w = dtoi(real_sz*pixel_res);
        int h = dtoi(imag_sz*pixel_res);

        init_screen(w, h);
        res = 1.0/pixel_res;
        ul = center + res*cplx(-(screen->w + 1)/2, (screen->h + 1)/2);
    }

    void width_set_window(int w, int h, double wdim, cplx center = 0.0)
    {
        set_window(wdim, (wdim*h)/w, w/wdim, center);
    }

    void height_set_window(int w, int h, double hdim, cplx center = 0.0)
    {
        set_window((hdim*w)/h, hdim, h/hdim, center);
    }

    mandel_ext_anim(int m, double b, double tol, double th) {
        init(m, b, tol, th);
    }

    mandel_ext_anim(bool b = true) {
        if (b) {
            init(300, 10, 1e-10, NAN);
            set_window(4, 3, 200, -0.5);

            set_title("Mandelbrot set with external angles");

            compute(24, 60, 1000, 3);
        }
    }
};

int main(int argc, char **argv) {
    try { 
        mandel_ext_anim();
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

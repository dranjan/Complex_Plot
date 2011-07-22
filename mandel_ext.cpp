#include "sdlm-v01.hpp"
#include "complex_plot.hpp"
#include "angle_tools.hpp"

#include <vector>
#include <iostream>
#include <algorithm>

/* struct pixel_data {
    double pre_arg;
    cplx u;
}; */

using namespace std;

class mandel_ext : public complex_plot {
protected:
    int dw;
    int dh;

    int sres;

    int max_iter;
    double ptol;
    double bailout;
    double logb;
    double theta;

    double safety;

    vector<cplx> data;

    bool fixed;

    friend class pixel_sorter;

    cplx to_plane_sub(double x, double y) {
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
        pair<int, int> p = from_plane_sub(c);
        if (fixed) return index(p.first, p.second);

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

                r.imag() = v; //fix_angle(v - repdbl(theta, n));
                break;
            }

            if (abs(z2-z) < ptol) {
                /* Looks like we hit a period */
                break;
            }
        }

        index(p.first, p.second) = r;

        return r;
    }

    color colormap(cplx z) {
        if (fixed) {
            if (isnan(z.imag())) {
                return RGBcolor(0.0, 0.0, 0.0);
            }

            int n = ceil(z.real());
            double r = n - z.real();
            r = n - r/safety;
            z.real() = log(r); //pow(2, -r)*logb;

            color c = basic_colormap(exp(z));
            double s = 0;

            if (abs(z.imag() - theta) <= 0.01) {
                s = (0.01 - abs(z.imag() - theta))/0.01;
            }

            return c*(1-s);

        } else {
            if (isnan(z.imag())) {
                return RGBcolor(1.0, 1.0, 1.0);
            } else {
                //z.real() = ceil(z.real());
                double r = 1-1.0/sqrt(log2(z.real()+2.5));
                if (z.imag() > 0) return RGBcolor(r, r, r);
                else return RGBcolor(1-0.5*(1-r), r, 1);
            }
        }
    }

    class sample_comp {
        const mandel_ext & parent;
    public:
        sample_comp(const mandel_ext & p) : parent(p)
        { }
        bool operator () (int kl, int kr) {
            return parent.data[kl].real() < parent.data[kr].real();
        }
    };

    void fix_pixels() {
        if (fixed) return;

        int num_samples = dw*dh;

        vector<int> sample_list(num_samples);
        vector<bool> sample_fixed(num_samples, false);

        for (int k = 0; k < num_samples; ++k) {
            sample_list[k] = k;
        }

        sort(sample_list.begin(), sample_list.end(),
             sample_comp(*this));

        int percent_complete = 0;
        if (verbosity > 0) std::cout << "0% complete.";

        for (int i = 0; i < num_samples; ++i) {
            int k = sample_list[i];

            int x = k % (dw);
            int y = k / (dw);

            if (data[k].real() == HUGE_VAL) break;

            int n = ceil(data[k].real());

            for (int dx = -1; dx < 2; ++dx) {
                for (int dy = -1; dy < 2; ++dy) {
                    int x1 = x + dx;
                    int y1 = y + dy;

                    if (x1 < 0 || x1 >= dw || y1 < 0 || y1 >= dh) {
                        continue;
                    }

                    int j = dw*y1 + x1;
                    
                    if (!sample_fixed[j]) continue;

                    double phi = data[j].imag();
                    double psi = div_angle(data[k].imag(), 
                                           pow(2, n), phi);
                    data[k].imag() = psi;

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
            
            int new_percent = 100*(k + 1)/num_samples;
            if (new_percent > percent_complete) {
                percent_complete = new_percent;
                if (verbosity > 0) {
                    cout << "\r" << percent_complete 
                         << "% complete.";
                    cout.flush();
                }
            }
        }
        
        if (verbosity > 0) cout << endl;

        fixed = true;
    } 

    void init_data(int w, int h) {
        dw = w;
        dh = h;
        data.resize(w*h);
    }

    void init(int m, double b, double tol, double th) {
        max_iter = m;
        bailout = b;
        logb = log(b);
        ptol = tol;
        theta = th;

        safety = 0.618033989;

        fixed = false;
    }

    virtual void mousebuttondown_event(SDL_MouseButtonEvent & button) {
        Uint16 x = button.x;
        Uint16 y = button.y;
        
        cplx c = to_plane(x+0.5, y+0.5);

        if (verbosity > 0) {
            cout << c << endl;
        }
    }

    virtual void keydown_event(SDL_KeyboardEvent & key) {
        SDLKey sym = key.keysym.sym;
        if (sym == SDLK_ESCAPE) {
            quit_event_loop = true;
        } else if (sym == SDLK_s) {
            dump_window(bmp_name.c_str());
        } else if (sym == SDLK_f) {
            if (!working) {
                if (verbosity > 0) {
                    cout << "Fixing angles..." << endl;
                    fix_pixels();
                    replot();
                }
            }
        }
    }

    void compute(int subsample_res = 1) {
        sres = subsample_res;

        init_data(screen->w*sres, screen->h*sres);

        run(sres);
    }

public:
    mandel_ext(int m, double b, double tol, double th) {
        init(m, b, tol, th);
    }

    mandel_ext(bool b = true) {
        if (b) {
            init(300, 10, 1e-10, M_PI/3);
            set_window(4, 3, 300, -0.5);

            set_title("??? Mandelbrot");
            bmp_name = "mandel_ext01.bmp";

            compute(3);
        }
    }
};

int main(int argc, char **argv) {
    try { 
        mandel_ext();
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

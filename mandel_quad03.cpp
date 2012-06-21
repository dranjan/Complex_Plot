#include <cassert>
#include <iostream>
#include <cmath>

#include "complex_plot.hpp"

using namespace std;

const cplx control[2][2] = {{cplx(-1.0,-1.0), cplx(-1.0, 1.0)}, 
                            {cplx( 1.0,-1.0), cplx( 1.0, 1.0)}};

const int nt = 5;
const cplx test[nt] = {cplx(1.0,0.0), cplx(-1.0,0.0), cplx(0.0,1.0), 
                       cplx(0.0,-1.0), cplx(0.0,0.0)};

inline cplx f(cplx z, cplx c) {
    return z*z + c;
}

struct iterate_data {
    /* Iterate values at the control points (vertices) */
    cplx cvals[2][2];

    /* Iterate values at the test points */
    cplx tvals[nt];

    /* Coefficients for the approximation */
    cplx a, b;

    /* Error approximation */
    double err;

    /* Number of iterations */
    int n;
};

struct quadtree_cell {
    /* Only square cells are currently supported */
    cplx center;
    double r;

    bool leaf;

    quadtree_cell *sub[2][2];

    /* Swappable buffers */
    iterate_data *iA;
    iterate_data *iB;

    /* Current error estimate */
    double err;

    /* Tally of total number of cells allocated */
    static int num_cells;

    bool subdivide() {
        if (!leaf) return false;

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                cplx c = center + r/2*cplx(2.0*i-1, 2.0*j-1);
                sub[i][j] = new quadtree_cell(c, r/2, this);
            }
        }

        /* Our own iterate data becomes obsolete after we subdivide */
        delete iA;
        delete iB;

        iA = 0;
        iB = 0;

        leaf = false;

        return true;
    }

    quadtree_cell *locate(cplx z) {
        cplx d = z - center;

        if (abs(d.real()) > r || abs(d.imag()) > r) {
            return 0;
        }

        if (leaf) {
            return this;
        }

        return sub[d.real() > 0][d.imag() > 0]->locate(z);
    }

    cplx control_point(int i, int j) const {
        return center + r*control[i][j];
    }

    cplx test_point(int k) const {
        return center + r*test[k];
    }

    cplx operator () (cplx z) const {
        assert( leaf );
        return (iA->a)*z + iA->b;
    }

    void compute_coeffs() {
        /* compute approximation coefficients */
        cplx s = 0.0;
        cplx t = 0.0;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                s += iA->cvals[i][j]*conj(control[i][j]);
                t += iA->cvals[i][j];
            }
        }
        
        iA->a = s/(8*r);
        iA->b = t/4.0 - center*(iA->a);

        /* approximate error */
        iA->err = 0;
        for (int k = 0; k < nt; ++k) {
            double t = abs((*this)(test_point(k)) - iA->tvals[k]);
            if (t > iA->err) iA->err = t;
        }
    }

    void swap_buffers() {
        iterate_data *t = iB;
        iB = iA;
        iA = t;
    }

    void step() {
        swap_buffers();

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                cplx c = control_point(i, j);
                cplx z = iB->cvals[i][j];
                iA->cvals[i][j] = f(z, c);
            }
        }

        for (int k = 0; k < nt; ++k) {
            cplx c = test_point(k);
            cplx z = iB->tvals[k];
            iA->tvals[k] = f(z, c);
        }

        iA->n = iB->n + 1;

        compute_coeffs();
    }

    bool check_step(double max_err) {
        step();

        if (iA->err > max_err) {
            swap_buffers();
            return false;
        }

        return true;
    }

    quadtree_cell *locate_deep(cplx z, int max_iter, double bailout,
                               double tol, double min_r)
    {
        quadtree_cell *p = locate(z);
        if (!p) return 0;

        /* Bug: what if *iB hasn't been initialized yet? 
         * (can this happen?)
         */
        if (p->iA->err > tol) {
            p->swap_buffers();
            p->subdivide();
            p = p->locate(z);
        } 

        while (p->iA->n < max_iter) {

            double a = HUGE_VAL;
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    double r = abs(p->iA->cvals[i][j]);
                    if (r < a) a = r;
                }
            }

            for (int k = 0; k < nt; ++k) {
                double r = abs(p->iA->tvals[k]);
                if (r < a) a = r;
            }

            if (a > bailout) {
                /* This cell has escaped */
                break;
            }

            p->step();

            if (p->iA->err > tol) {
                if (p->r > min_r) {
                    p->swap_buffers();
                    p->subdivide();
                    p = p->locate(z);
                } else {
                    /* This cell is small enough already that
                     * approximation error doesn't really matter;
                     * just continue
                     */
                }
            }
        }

        return p;
    }

    double smooth_count(cplx z, double bailout) {
        double c = iA->n;
        c -= log2(log(abs((*this)(z)))/log(bailout));
        return c;
    }

    quadtree_cell(cplx c, double d, const quadtree_cell * parent = 0) 
        : center(c), r(d), leaf(true)
    {
        ++num_cells;

        sub[0][0] = sub[0][1] = sub[1][0] = sub[1][1] = 0;
        
        iA = new iterate_data();
        iB = new iterate_data();

        if (parent) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    iA->cvals[i][j] = (*parent)(control_point(i, j));
                }
            }

            for (int k = 0; k < nt; ++k) {
                iA->tvals[k] = (*parent)(test_point(k));
            }

            iA->n = parent->iA->n;
        } else {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    iA->cvals[i][j] = 0.0;
                }
            }

            for (int k = 0; k < nt; ++k) {
                iA->tvals[k] = 0.0;
            }

            iA->n = 0;
        }

        compute_coeffs();
    }

    virtual ~quadtree_cell() {
        if (!leaf) {
            delete sub[0][0]; delete sub[0][1];
            delete sub[1][0]; delete sub[1][1];
        } else {
            delete iA;
            delete iB;
        }
    }
};

int quadtree_cell::num_cells = 0;

class mandel_quad : public complex_plot {
protected:
    quadtree_cell q;

    double b;
    int max_iter;

    color color_func(cplx z) {
        quadtree_cell *p = q.locate_deep(z, max_iter, b, 1e-8, res/sres);
        if (!p) return RGBcolor(0, 0, 0);
        if (p->iA->n == max_iter) return RGBcolor(1, 1, 1);
        double r = p->smooth_count(z, b);
        return HSVcolor(mod(r,2*M_PI),1,1);
    }
public:
    mandel_quad(cplx center, double r, int px) : q(center, r)
    {
        set_window(r, r, px/r, center);

        set_title("Quadtree Mandelbrot");

        b = 10;
        max_iter = 10000;

        run(2);
    }
};

int main(int argc, char **argv) {
    try { 
        //mandel_quad(-0.5, 4, 400);
        //mandel_quad(cplx(-0.255,0.695), 4.0/27, 400);
        mandel_quad(cplx(-1.769110375463767385, 0.009020388228023440),
                    0.00000000000000008,
                    400);
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

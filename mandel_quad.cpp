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

struct quadtree_cell {
    /* Only square cells are currently supported */
    cplx center;
    double r;

    bool leaf;

    quadtree_cell *sub[2][2];

    /* Iterate values at the control points (vertices) */
    cplx cvals[2][2];

    /* Iterate values at the test points */
    cplx tvals[nt];

    int n;

    static int num_cells;

    bool subdivide() {
        if (!leaf) return false;

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                cplx c = center + r/2*cplx(2.0*i-1, 2.0*j-1);
                sub[i][j] = new quadtree_cell(c, r/2, this);
            }
        }

        leaf = false;

        return true;
    }

    const quadtree_cell *locate(cplx z) const {
        cplx d = z - center;

        //cout << z << center << r << endl;

        if (abs(d.real()) > r || abs(d.imag()) > r) {
            //cout << "outside." << endl;
            return 0;
        }

        if (leaf) {
            //cout << "found." << endl;
            return this;
        }

        //cout << "recursing..." << endl;
        return sub[d.real() > 0][d.imag() > 0]->locate(z);
    }

    cplx control_point(int i, int j) const {
        return center + r*control[i][j];
    }

    cplx test_point(int k) const {
        return center + r*test[k];
    }

    /* This is just bilinear interpolation from the 
     * vertices. TODO: It would probably make more sense
     * to interpolate by a holomorphic function (like a 
     * complex polynomial, or at least a rational function).
     */
    cplx operator () (cplx z) const {
        double a = ((z.real() - center.real())/r + 1)/2;
        double b = ((z.imag() - center.imag())/r + 1)/2;

        return (1-a)*((1-b)*cvals[0][0] + b*cvals[0][1]) +
                   a*((1-b)*cvals[1][0] + b*cvals[1][1]);
    }

    void step() {
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                cplx c = control_point(i, j);
                cplx z = cvals[i][j];
                cvals[i][j] = f(z, c);
            }
        }

        for (int k = 0; k < nt; ++k) {
            cplx c = test_point(k);
            cplx z = tvals[k];
            tvals[k] = f(z, c);
        }

        ++n;
    }

    double badness() const {
        double b = 0;

        /*for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                cout << cvals[i][j] << " ";
            }
        }
        cout << endl;

        for (int k = 0; k < nt; ++k) {
            cout << tvals[k] << " ";
        }
        cout << endl;*/

        for (int k = 0; k < nt; ++k) {
            double t = abs((*this)(test_point(k)) - tvals[k]);
            if (t > b) b = t;
        }

        return b;
    }

    /* TODO: This is very inefficient. Double-buffering would be a
     * better way to do this, but it requires smarter management.
     */
    bool check_step(double max_err) {
        cplx cvals_save[2][2];
        cplx tvals_save[nt];

        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                cvals_save[i][j] = cvals[i][j];
            }
        }

        for (int k = 0; k < nt; ++k) {
            tvals_save[k] = tvals[k];
        }

        step();

        if (badness() > max_err) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    cvals[i][j] = cvals_save[i][j];
                }
            }

            for (int k = 0; k < nt; ++k) {
                tvals[k] = tvals_save[k];
            }

            --n;

            return false;
        }

        return true;
    }

    /* TODO: Implement periodicity checking. Again, this would
     * require smarter bookkeeping. 
     */
    void iterate(double bailout, int max_iter, double max_err) {
        while (n < max_iter) {
            assert( leaf );

            double a = HUGE_VAL;
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    if (abs(cvals[i][j]) < a) a = abs(cvals[i][j]);
                }
            }

            for (int k = 0; k < nt; ++k) {
                if (abs(tvals[k]) < a) a = abs(tvals[k]);
            }

            if (a > bailout) {
                /* This cell has escaped */
                return;
            }

            if (!check_step(max_err)) {
                subdivide();
                for (int i = 0; i < 2; ++i) {
                    for (int j = 0; j < 2; ++j) {
                        sub[i][j]->iterate(bailout, max_iter, max_err);
                    }
                }

                return;
            }
        }
    }

    quadtree_cell(cplx c, double d, const quadtree_cell * parent = 0) 
        : center(c), r(d), leaf(true)
    {
        ++num_cells;
        //cout << c << r << endl;

        sub[0][0] = sub[0][1] = sub[1][0] = sub[1][1] = 0;
        
        if (parent) {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    cvals[i][j] = (*parent)(control_point(i, j));
                }
            }

            for (int k = 0; k < nt; ++k) {
                tvals[k] = (*parent)(test_point(k));
            }

            n = parent->n;
        } else {
            for (int i = 0; i < 2; ++i) {
                for (int j = 0; j < 2; ++j) {
                    cvals[i][j] = 0.0;
                }
            }

            for (int k = 0; k < nt; ++k) {
                tvals[k] = 0.0;
            }

            n = 0;
        }
    }

    virtual ~quadtree_cell() {
        if (!leaf) {
            delete sub[0][0]; delete sub[0][1];
            delete sub[1][0]; delete sub[1][1];
        }
    }
};

int quadtree_cell::num_cells = 0;

class mandel_quad : public complex_plot {
protected:
    quadtree_cell q;

    cplx transform(cplx z) {
        const quadtree_cell *p = q.locate(z);
        if (!p) return cplx(NAN, NAN);
        return (*p)(z);
        //return q(z);
    }
public:
    mandel_quad(cplx center, double r, int px) : q(center, r)
    {
        set_window(r, r, px/r, center);

        set_title("Quadtree Mandelbrot");

        //q.step();
        //q.step();
        //cout << q.badness() << endl;
        q.iterate(10, 3, 0.005);
        /*q.step();
        q.step();
        q.subdivide();
        q.sub[0][0]->subdivide();
        q.sub[0][1]->subdivide();
        q.sub[1][0]->subdivide();
        q.sub[1][1]->subdivide();*/

        //q.step();
        cout << quadtree_cell::num_cells << " cells" << endl;

        run(2);
    }
};

int main(int argc, char **argv) {
    try { 
        mandel_quad(-0.5, 4, 400);
    } catch (std::string s) {
        cerr << "Error: " << s << endl;
        return 1;
    }

    return 0;
}

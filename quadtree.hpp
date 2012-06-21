#ifndef MAND_HPP
#define QUADTREE_HPP

template <typename T>
struct quadtree {
    T data;

    double xL, xR, yL, yR;
    bool leaf;

    double xM, yM;

    quadtree *branches[2][2];

    quadtree<T>(double x0, double x1, double y0, double y1,
                const T & d = T())
        : data(d), xL(x0), xR(x1), yL(y0), yR(y1), leaf(true)
    {
        xM = xL + (xR-xL)/2;
        yM = yL + (yR-yL)/2;

        branches[0][0] = 0;
        branches[0][1] = 0;
        branches[1][0] = 0;
        branches[1][1] = 0;
    }

    virtual ~quadtree<T>() {
        if (!leaf) {
            delete branches[0][0];
            delete branches[0][1];
            delete branches[1][0];
            delete branches[1][1];
        }
    }

    bool is_leaf() {
        return leaf;
    }

    bool subdivide(const T & d = T()) {
        if (!leaf) return false;

        double xM = xL + (xR-xL)/2;
        double yM = yL + (yR-yL)/2;

        branches[0][0] = new quadtree(xL, xM, yL, yM, d);
        branches[0][1] = new quadtree(xL, xM, yM, yR, d);
        branches[1][0] = new quadtree(xM, xR, yL, yM, d);
        branches[1][1] = new quadtree(xM, xR, yM, yR, d);

        leaf = false;
    }

    quadtree *branch(bool i, bool j) {
        return branches[i][j];
    }

    quadtree *locate(double x, double y) {
        if (x < xL || x > xR || y < yL || y > yR) return 0;

        if (leaf) return this;

        return branches[x >= xM][y >= yM]->locate(x, y);
    }
};

#endif

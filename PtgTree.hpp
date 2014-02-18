#ifndef __PTG_TREE__H__
#define __PTG_TREE__H__

#include "skel/skel.h"
#include "PtgSquare.hpp"

#include <cmath>

enum ColorMode {
    CM_RAINBOW, CM_TREE
};

enum AngleMode {
    AM_FIXED, AM_RANDOM
};

class PtgTree {
    private:
        std::vector<std::vector<PtgSquare> > _sqrs;
        GLfloat _angle;

        ColorMode _cm;
        AngleMode _am;

        void setLevelColor (int level);

    public:
        PtgTree ();
        PtgTree (GLdouble x, GLdouble y);
        ~PtgTree () {}

        void colorMode (ColorMode cm) { _cm = cm; }
        void angleMode (AngleMode am) { _am = am; }

        void rotate (GLfloat angle);
        void grow ();
        void shrink ();

        void regrow ();

        void draw ();

        PtgSquare getSmallestSquareAt (Vect pt);
};

#endif
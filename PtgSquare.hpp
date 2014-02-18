#ifndef __PTG_SQUARE__H__
#define __PTG_SQUARE__H__

#include "skel/skel.h"

#include <utility>

class PtgSquare {
    private:
        GLfloat _x, _y;
        GLfloat _size;
        GLfloat _ang;

    public:
        PtgSquare (GLfloat x, GLfloat y, GLfloat size, GLfloat ang)
            : _x(x), _y(y), _size(size), _ang(ang) {}
        ~PtgSquare () {}

        void x (GLfloat x) { _x = x; }
        void y (GLfloat y) { _y = y; }
        void size (GLfloat size) { _size = size; }
        void ang (GLfloat ang) { _ang = ang; }

        GLfloat x () { return _x; }
        GLfloat y () { return _y; }
        GLfloat size () { return _size; }
        GLfloat ang () { return _ang; }

        void draw ();

        std::pair<PtgSquare,PtgSquare> nextLevel (GLfloat angle);

        bool isPointInside (Vect pt);
};

#endif
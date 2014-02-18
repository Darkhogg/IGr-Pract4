#include "PtgSquare.hpp"

#include <math.h>

void PtgSquare::draw () {
    Turtle t (x(), y(), ang());

    glBegin(GL_QUADS);
    for (int i = 0; i < 4; i++) {
        t.advance(_size);
        t.rotate(M_PI_2);
        glVertex2d(t.x(), t.y());
    }
    glEnd();
}

std::pair<PtgSquare,PtgSquare> PtgSquare::nextLevel (GLfloat angle) {
    Turtle t (x(), y(), ang());
    t.rotate(M_PI_2);
    t.advance(size());
    t.rotate(-angle);

    GLfloat leftSize = size()*sin(angle);
    PtgSquare left (t.x(), t.y(), leftSize, t.ang());

    t.advance(leftSize);
    t.rotate(-M_PI_2);

    PtgSquare right (t.x(), t.y(), size()*cos(angle), t.ang());
    return std::pair<PtgSquare,PtgSquare>(left, right);
}

bool PtgSquare::isPointInside(Vect pt) {
    if (pow(pt.x()-_x, 2) + pow(pt.y()-_y, 2) > pow(_size*1.41, 2)) {
        return false; // "fast" discard
    }

    Turtle sqt (x(), y(), ang());
    for (int i = 0; i < 4; i++) {
        Turtle ot (sqt.x(), sqt.y());
        ot.lookAt(pt.x(), pt.y());

        GLfloat ad = ot.ang() >= sqt.ang()
            ? ot.ang() - sqt.ang()
            : ot.ang() + M_PI + M_PI - sqt.ang();
        
        if (ad > M_PI) {
            return false;
        }

        sqt.advance(_size);
        sqt.rotate(M_PI_2);
    }

    return true;
}
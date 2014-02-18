#include "PtgTree.hpp"

PtgTree::PtgTree () : _angle(M_PI_4), _cm(CM_RAINBOW), _am(AM_FIXED) {
    // Initialize the first level...
    std::vector<PtgSquare> lvl0;
    lvl0.push_back(PtgSquare(-50.0, -200.0, 100, 0.0));

    // ...and insert it
    _sqrs.push_back(lvl0);

    // Grow 8 levels
    for (int i = 0; i < 8; i++) {
        grow();
    }
}

PtgTree::PtgTree (GLdouble x, GLdouble y) : _angle(M_PI_4), _cm(CM_RAINBOW), _am(AM_FIXED) {
    // Initialize the first level...
    std::vector<PtgSquare> lvl0;
    lvl0.push_back(PtgSquare(x, y, 100, 0.0));

    // ...and insert it
    _sqrs.push_back(lvl0);

    // Grow 8 levels
    for (int i = 0; i < 8; i++) {
        grow();
    }
}

void PtgTree::rotate (GLfloat angle) {
    _angle += angle;
    if (_angle < 0.5) {
        _angle = 0.5;
    }
    if (_angle > M_PI_2 - 0.5) {
        _angle = M_PI_2 - 0.5;
    }
}

void PtgTree::grow () {
    const int next = _sqrs.size();
    const int prev = next - 1;

    _sqrs.push_back(std::vector<PtgSquare>());

    for (std::vector<PtgSquare>::iterator it = _sqrs[prev].begin(); it != _sqrs[prev].end(); ++it) {
        GLfloat angle = _angle;
        if (_am == AM_RANDOM) {
            angle += (rand() / (double) RAND_MAX) * M_PI_4 - (M_PI_4/2);
        }

        std::pair<PtgSquare,PtgSquare> nexts = it->nextLevel(angle);
        _sqrs[next].push_back(nexts.first);
        _sqrs[next].push_back(nexts.second);
    }
}

void PtgTree::shrink () {
    if (_sqrs.size() > 1) {
        _sqrs.pop_back();
    }
}

void PtgTree::regrow () {
    int size = _sqrs.size();

    while (_sqrs.size() > 1) {
        shrink();
    }

    while (_sqrs.size() < size) {
        grow();
    }
}

GLdouble interpolate (int lvl, int step, int color) {
    int c = ((lvl/step - color*2) % 6 + 6) % 6;
    int m = (lvl % step + step) % step;

    switch (c) {
        case 0: return 1.0;
        case 1: return 1.0;
        case 2: return (GLdouble) (step - m) / step;
        case 3: return 0.0;
        case 4: return 0.0;
        case 5: return (GLdouble) m / step;
    }
}

void PtgTree::setLevelColor (int level) {
    GLdouble r, g, b;

    switch (_cm) {
        case CM_RAINBOW: {
            const int step = 3;
            r = interpolate(level, step, 0);
            g = interpolate(level, step, 1);
            b = interpolate(level, step, 2);

            break;
        }

        case CM_TREE: {
            r = 0.45f - 0.3 * log(level/8.0);
            g = 0.95f - 0.55 * exp(-level/12.0);
            b = 0.1f;

            break;
        }
    }

    glColor3d(r, g, b);
}

void PtgTree::draw () {
    int idxInit = 0;//floor(zoom() / 1);
    int idxEnd = 20;//idxInit + 10;

    for (int i = std::max(0, idxInit); i < idxEnd &&i < _sqrs.size(); i++) {
        setLevelColor(i);

        for (std::vector<PtgSquare>::iterator lit = _sqrs[i].begin(); lit != _sqrs[i].end(); ++lit) {
            lit->draw();
        }
    }
}

PtgSquare PtgTree::getSmallestSquareAt (Vect pt) {
    PtgSquare sel (0, 0, 0, 0);

    for (int i = _sqrs.size() - 1; i >= 0; i--) {
        for (int j = 0; j < _sqrs[i].size(); j++) {
            PtgSquare sqr = _sqrs[i][j];

            if ((sel.size() <= 0 || sel.size() > sqr.size())
             && sqr.isPointInside(pt))
            {
                sel = sqr;
            }
        }
    }

    return sel;
}
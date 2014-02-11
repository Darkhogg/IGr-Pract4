#include "Pixbuf.hpp"

#include <iostream>

#include "skel/skel.h"

pixel Pixbuf::operator() (std::size_t x, std::size_t y) const {
  return _buffer[x + y * _width];
}

pixel& Pixbuf::operator() (std::size_t x, std::size_t y) {
  return _buffer[x + y * _width];
}


Pixbuf Pixbuf::rotated (Vect center, GLdouble angle) {
  auto c = center - Vect{_width/2, -_height/2, 0.d};

  Pixbuf target {_width, _height};

  for (std::size_t tx = 0; x < target.width(); ++x) {
    for (std::size_t ty = 0; y < target.height(); ++y) {
      auto ox = tx; // FIXME  
      auto oy = ty; // FIXME  

      
    }
  }

  return target;
}


void Pixbuf::rotate (Vect center, GLdouble angle) {
  Pixbuf r = rotated(center, angle);
  (*this) = r;
}


void Pixbuf::draw () const {
  GLint w2 = - _width / 2;
  GLint h2 = _height / 2;

  glBegin(GL_POINTS);
  for (std::size_t i = 0; i < _width; ++i) {
    for (std::size_t j = 0; j < _height; ++j) {
      pixel p = (*this)(i, j);
      glColor3f(p.red / 255.f, p.green / 255.f, p.blue / 255.f);
      glVertex2i(w2 + i, h2 - j);
    }
  }
  glEnd();
}

void Pixbuf::desaturate () {
  for (std::size_t k = 0; k < _buffer.size(); ++k) {
    pixel px = _buffer[k];
    pixel::color y = (pixel::color) (0.299 * px.red + 0.586 * px.green + 0.114 * px.blue);
    _buffer[k] = {y, y, y, px.alpha};
  }
}

std::ostream& operator<< (std::ostream& s, const pixel px) {
  return s
    << "["  << (int) px.red
    << ", " << (int) px.green
    << ", " << (int) px.blue
    << "; " << (int) px.alpha
    << "]";
}
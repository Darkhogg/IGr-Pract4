#include "Pixbuf.hpp"

#include <iostream>

#include "skel/skel.h"

pixel Pixbuf::operator() (std::size_t x, std::size_t y) const {
  return _buffer[x + y * _width];
}

pixel& Pixbuf::operator() (std::size_t x, std::size_t y) {
  return _buffer[x + y * _width];
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

std::ostream& operator<< (std::ostream& s, const pixel px) {
  return s
    << "["  << (int) px.red
    << ", " << (int) px.green
    << ", " << (int) px.blue
    << "; " << (int) px.alpha
    << "]";
}
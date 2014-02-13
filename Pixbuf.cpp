#include "Pixbuf.hpp"

#include <iostream>
#include <array>
#include <cassert>

#include "skel/skel.h"

void Pixbuf::range_check (std::size_t x, std::size_t y) const {
  assert (x < _width);
  assert (y < _height);
}

pixel Pixbuf::operator() (std::size_t x, std::size_t y) const {
  range_check(x, y);
  return _buffer[x + y * _width];
}

pixel& Pixbuf::operator() (std::size_t x, std::size_t y) {
  range_check(x, y);
  return _buffer[x + y * _width];
}


pixel Pixbuf::color_at (int x, int y) const {
  if (x < 0 || y < 0 || x >= _width || y >= _height) {
    return {0, 0, 0, 0};
  }

  return (*this)(x, y);
}

pixel Pixbuf::color_at (double x, double y) const {
  int lx = (int) x;
  int rx = lx + 1;
  int ty = (int) y;
  int by = y + 1;

  // Pixels surrounding position
  std::array<pixel, 4> pxs {
    color_at(lx, ty),
    color_at(lx, by),
    color_at(rx, ty),
    color_at(rx, by)
  };

  // Weights of said pixel
  auto wx = x - lx;
  auto wy = y - ty;

  std::array<double, 4> ws {
    ((1 - wx) + (1 - wy)) * 0.25,
    ((1 - wx) +      wy ) * 0.25,
    (     wx  + (1 - wy)) * 0.25,
    (     wx  +      wy ) * 0.25
  };

  double r, g, b, a;
  for (std::size_t i = 0; i < 4; ++i) {
    pixel px = pxs[i];
    double w = ws[i];

    r += w * px.red;
    g += w * px.green;
    b += w * px.blue;
    a += w * px.alpha;
  }

  return {(pixel::color) r, (pixel::color) g, (pixel::color) b, (pixel::color) a};
}



Pixbuf Pixbuf::rotated (Vect center, GLdouble angle) const {
  Vect c {
    center.x() + _width / 2,
    center.y() - _height / 2,
    0.0
  };

  Pixbuf target {_width, _height};

  for (std::size_t tx = 0; tx < target.width(); ++tx) {
    for (std::size_t ty = 0; ty < target.height(); ++ty) {
      auto dx = tx - c.x();
      auto dy = ty - c.y();

      auto ang = atan2(dy, dx) + angle;
      auto len = sqrt(dx * dx + dy * dy);

      double ox = c.x() + len * cos(ang);
      double oy = c.y() + len * sin(ang);

      target(tx, ty) = color_at(ox, oy);
    }
  }

  return target;
}


Pixbuf Pixbuf::resized_canvas (std::size_t new_width, std::size_t new_height) const {
  Pixbuf target {new_width, new_height};

  for (std::size_t tx = 0; tx < target.width(); ++tx) {
    for (std::size_t ty = 0; ty < target.height(); ++ty) {
      target(tx, ty) = color_at(
        (double) tx + (new_width - _width) / 2,
        (double) ty + (new_height - _height) / 2
      );
    }
  }

  return target;
}

Pixbuf Pixbuf::resized_image (std::size_t new_width, std::size_t new_height) const {
  Pixbuf target {new_width, new_height};

  for (std::size_t tx = 0; tx < target.width(); ++tx) {
    for (std::size_t ty = 0; ty < target.height(); ++ty) {
      target(tx, ty) = color_at(
        (double) tx * _width / target.width(),
        (double) ty * _height / target.height()
      );
    }
  }

  return target;
}


void Pixbuf::rotate (Vect center, GLdouble angle) {
  (*this) = rotated(center, angle);
}

void Pixbuf::resize_canvas (std::size_t new_width, std::size_t new_height) {
  (*this) = resized_canvas(new_width, new_height);
}

void Pixbuf::resize_image (std::size_t new_width, std::size_t new_height) {
  (*this) = resized_image(new_width, new_height);
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
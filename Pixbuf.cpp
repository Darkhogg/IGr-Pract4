#include "Pixbuf.hpp"

#include <iostream>
#include <array>
#include <vector>
#include <cassert>
#include <cmath>
#include <algorithm>

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
  invalidate_texture();
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
    ((1 - wx) * (1 - wy)),
    ((1 - wx) *      wy ),
    (     wx  * (1 - wy)),
    (     wx  *      wy )
  };

  double r, g, b, a;
  for (std::size_t i = 0; i < 4; ++i) {
    pixel px = pxs[i];
    double w = ws[i];

    r += (w * px.red) / 1;
    g += (w * px.green) / 1;
    b += (w * px.blue) / 1;
    a += (w * px.alpha) / 1;
  }

  return {(pixel::color) r, (pixel::color) g, (pixel::color) b, (pixel::color) a};
}



Pixbuf Pixbuf::rotated (Vect center, GLdouble angle) const {
  Vect c {
    center.x() + _width / 2,
    - center.y() + _height / 2,
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
      int wd2 = ((int) target.width() - (int) _width) / 2;
      int hd2 = ((int) target.height() - (int) _height) / 2;

      target(tx, ty) = color_at((int) tx - wd2, (int) ty - hd2);
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

Pixbuf Pixbuf::mixed (Pixbuf other, GLdouble factor) const {
  Pixbuf target {_width, _height};
  Pixbuf otherr = other.resized_image(_width, _height);

  auto f1 = 1 - factor;
  auto f2 = factor;

  for (std::size_t x = 0; x < target.width(); ++x) {
    for (std::size_t y = 0; y < target.height(); ++y) {
      pixel p1 = (*this)(x, y);
      pixel p2 = otherr(x, y);

      target(x, y) = {
        (pixel::color) (f1 * p1.red   + f2 * p2.red),
        (pixel::color) (f1 * p1.green + f2 * p2.green),
        (pixel::color) (f1 * p1.blue  + f2 * p2.blue),
        (pixel::color) (f1 * p1.alpha + f2 * p2.alpha)
      };
    }
  }

  return target;
}



void Pixbuf::rotate (Vect center, GLdouble angle) {
  (*this) = rotated(center, angle);
  invalidate_texture();
}

void Pixbuf::resize_canvas (std::size_t new_width, std::size_t new_height) {
  (*this) = resized_canvas(new_width, new_height);
  invalidate_texture();
}

void Pixbuf::resize_image (std::size_t new_width, std::size_t new_height) {
  (*this) = resized_image(new_width, new_height);
  invalidate_texture();
}

void Pixbuf::mix (Pixbuf other, GLdouble factor) {
  (*this) = mixed(other, factor);
  invalidate_texture();
}


Pixbuf Pixbuf::sobel () const {
  // Target pixbuf to return
  Pixbuf target {_width, _height};

  // Original desaturated buffer
  Pixbuf orig {*this};
  orig.desaturate();


  for (std::size_t x = 0; x < _width; ++x) {
    for (std::size_t y = 0; y < _height; ++y) {
      int z1 = (int) orig.color_at((int) x - 1, (int) y - 1).red;
      int z2 = (int) orig.color_at((int) x    , (int) y - 1).red;
      int z3 = (int) orig.color_at((int) x + 1, (int) y - 1).red;
      int z4 = (int) orig.color_at((int) x - 1, (int) y    ).red;
      int z5 = (int) orig.color_at((int) x    , (int) y    ).red;
      int z6 = (int) orig.color_at((int) x + 1, (int) y    ).red;
      int z7 = (int) orig.color_at((int) x - 1, (int) y + 1).red;
      int z8 = (int) orig.color_at((int) x    , (int) y + 1).red;
      int z9 = (int) orig.color_at((int) x + 1, (int) y + 1).red;

      int gx = (z1 + 2 * z4 + z7) - (z3 + 2 * z6 + z9);
      int gy = (z7 + 2 * z8 + z9) - (z1 + 2 * z2 + z3);

      int g = abs(gx) + abs(gy);
      pixel::color v = (pixel::color)
        ( g < 100 ? 0
        : g < 800 ? ((g - 100) * 255) / 700
        : 255
      ); 

      target(x, y) = {v, v, v};
    }
  }

  // Return the buffer
  return target;
}

void Pixbuf::add_sobel () {
  Pixbuf sbl = sobel();

  for (std::size_t x = 0; x < _width; ++x) {
    for (std::size_t y = 0; y < _height; ++y) {
      pixel sp = sbl(x, y);
      pixel op = (*this)(x, y);

      op.red = op.red / 3;
      op.green = op.green / 3;
      op.blue = op.blue / 3;

      op.red   = op.red   + ((sp.red   - op.red)   * sp.red) / 255;
      op.green = op.green + ((sp.green - op.green) * sp.red) / 255;
      op.blue  = op.blue  + ((sp.blue  - op.blue)  * sp.red) / 255;

      (*this)(x, y) = op;
    }
  }

  invalidate_texture();
}


void Pixbuf::desaturate () {
  for (std::size_t k = 0; k < _buffer.size(); ++k) {
    pixel px = _buffer[k];
    pixel::color y = (pixel::color) (0.299 * px.red + 0.586 * px.green + 0.114 * px.blue);
    _buffer[k] = {y, y, y, px.alpha};
  }

  invalidate_texture();
}


void Pixbuf::diff (Pixbuf other) {
  for (std::size_t k = 0; k < _buffer.size(); ++k) {
    pixel px = _buffer[k];
    pixel opx = other._buffer[k];

    px.red = (pixel::color) abs((int) px.red - (int) opx.red);
    px.green = (pixel::color) abs((int) px.green - (int) opx.green);
    px.blue = (pixel::color) abs((int) px.blue - (int) opx.blue);
    px.alpha = (pixel::color) abs((int) px.alpha - (int) opx.alpha);

    _buffer[k] = px;
  }

  invalidate_texture();
}


pixel Pixbuf::gaussian_average_at (double stdev, int x, int y) const {
  // 21*21 = 441
  double sumw = 0;
  std::array<double, GAUSS_SIZE> weights;
  std::array<pixel, GAUSS_SIZE> pixels;


  std::size_t idx = 0;
  for (int tx = x - GAUSS_RANGE; tx <= x + GAUSS_RANGE; ++tx) {
    for (int ty = y - GAUSS_RANGE; ty <= y + GAUSS_RANGE; ++ty) {
      auto dx = x - tx;
      auto dy = y - ty;
      double w = exp(-((dx * dx + dy * dy) / (2 * stdev * stdev)));

      pixels[idx] = color_at(tx, ty);
      weights[idx] = w;

      sumw += w;
      ++idx;
    }
  }

  double r = 0, g = 0, b = 0, a = 0;
  for (std::size_t i = 0; i < GAUSS_SIZE; ++i) {
    r += pixels[i].red * weights[i] / sumw;
    g += pixels[i].green * weights[i] / sumw;
    b += pixels[i].blue * weights[i] / sumw;
    a += pixels[i].alpha * weights[i] / sumw;
  }

  return {(pixel::color) r, (pixel::color) g, (pixel::color) b, (pixel::color) a};
}

Pixbuf Pixbuf::gaussian_smoothed (double stdev) const {
  Pixbuf target {_width, _height};

  for (std::size_t x = 0; x < target.width(); ++x) {
    for (std::size_t y = 0; y < target.height(); ++y) {
      target(x, y) = gaussian_average_at(stdev, (int) x, (int) y);
    }
  }

  return target;
}


void Pixbuf::gaussian_smooth (double stdev) {
  (*this) = gaussian_smoothed(stdev);
  invalidate_texture();
}

std::ostream& operator<< (std::ostream& s, const pixel px) {
  return s
    << "["  << (int) px.red
    << ", " << (int) px.green
    << ", " << (int) px.blue
    << "; " << (int) px.alpha
    << "]";
}


void Pixbuf::draw () {
  update_texture();

  if (_texid > 0) {
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, _texid );

    auto w2 = _width / 2;
    auto h2 = _height / 2;

    glBegin( GL_QUADS );
    glTexCoord2d(0.0, 0.0); glVertex2i(-w2, h2);
    glTexCoord2d(1.0, 0.0); glVertex2i(w2, h2);
    glTexCoord2d(1.0, 1.0); glVertex2i(w2, -h2);
    glTexCoord2d(0.0, 1.0); glVertex2i(-w2, -h2);
    glEnd();
  }
}

void Pixbuf::update_texture () {
  if (_texid == 0 && _width > 0 && _height > 0) {
    glGenTextures(1, &_texid);

    std::cout << "[GENTEX:" << _texid << "]" << std::endl;

    glBindTexture( GL_TEXTURE_2D, _texid );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );

    // when texture area is large, bilinear filter the original
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // the texture wraps over at the edges (repeat)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    std::cout << "[UPDTEX:" << _texid << "] " << std::endl;

    std::vector<unsigned char> dat {};

    for (std::size_t y = 0; y < _height; ++y) {
      for (std::size_t x = 0; x < _width; ++x) {
        pixel px = color_at((int) x, (int) y);

        std::size_t i = 3 * (x + y * _width); 

        dat.push_back(px.red);
        dat.push_back(px.green);
        dat.push_back(px.blue);
      }
    }

    glBindTexture( GL_TEXTURE_2D, _texid );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, dat.data() );
  }
}

void Pixbuf::invalidate_texture () {
  if (_texid > 0) {
    std::cout << "[DELTEX:" << _texid << "]" << std::endl;
    glDeleteTextures( 1, &_texid );
    _texid = 0;
  }
}
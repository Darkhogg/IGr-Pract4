#ifndef __PIXBUF__H__
#define __PIXBUF__H__

#include "skel/skel.h"

#include <cstddef>
#include <vector>

#include <iostream>

struct pixel {
  using color = unsigned char;

  color alpha, red, green, blue;

  pixel () : pixel{0, 0, 0} {}

  pixel (color r, color g, color b)
    : pixel{0, 0, 0, 255} {}

  pixel (color r, color g, color b, color a)
    : red{r}, green{g}, blue{b}, alpha{a} {}
};

std::ostream& operator<< (std::ostream& s, const pixel px);

class Pixbuf {
  private:
    std::size_t _width, _height;
    std::vector<pixel> _buffer;

    void range_check(std::size_t, std::size_t) const;

    pixel color_at (int x, int y) const;
    pixel color_at (double x, double y) const;

  public:
    Pixbuf () : Pixbuf(0, 0) {}
    Pixbuf (std::size_t width, std::size_t height)
      : _width{width}, _height{height}, _buffer{width * height} {}

    std::size_t width () const { return _width; }
    std::size_t height () const { return _height; }

    Pixbuf resized_canvas (std::size_t new_width, std::size_t new_height) const;
    void   resize_canvas  (std::size_t new_width, std::size_t new_height);

    Pixbuf resized_image (std::size_t new_width, std::size_t new_height) const;
    void   resize_image  (std::size_t new_width, std::size_t new_height);

    Pixbuf rotated (Vect center, GLdouble angle) const;
    void   rotate  (Vect center, GLdouble angle);

    pixel  operator() (std::size_t x, std::size_t y) const;
    pixel& operator() (std::size_t x, std::size_t y);

    void desaturate ();

    void draw () const;
};



#endif
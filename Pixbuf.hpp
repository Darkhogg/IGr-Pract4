#ifndef __PIXBUF__H__
#define __PIXBUF__H__

#include "skel/skel.h"

#include <cstddef>
#include <vector>

#include <iostream>

#define GAUSS_STDEV 1.5
#define GAUSS_RANGE 15
#define GAUSS_RANGE2 (GAUSS_RANGE * 2 + 1)
#define GAUSS_SIZE (GAUSS_RANGE2*GAUSS_RANGE2)

struct pixel {
  using color = unsigned char;

  color alpha, red, green, blue;

  pixel () : pixel{0, 0, 0} {}

  pixel (color r, color g, color b)
    : pixel{r, g, b, 255} {}

  pixel (color r, color g, color b, color a)
    : red{r}, green{g}, blue{b}, alpha{a} {}
};

std::ostream& operator<< (std::ostream& s, const pixel px);

class Pixbuf {
  private:
    std::size_t _width, _height;
    std::vector<pixel> _buffer;

    std::size_t _txmin, _txmax, _tymin, _tymax;
    GLuint _texid;

    void range_check(std::size_t, std::size_t) const;

    pixel color_at (int x, int y) const;
    pixel color_at (double x, double y) const;

    pixel gaussian_average_at (double stdev, int x, int y) const;

    void update_texture ();
    void invalidate_texture ();

  public:
    Pixbuf () : Pixbuf(0, 0) {}
    Pixbuf (std::size_t width, std::size_t height)
      : _width{width}, _height{height}, _buffer{width * height}, _texid{0},
        _txmin{0}, _txmax{width}, _tymin{0}, _tymax{height}
      {}
    ~Pixbuf () {
      invalidate_texture();
    }

    std::size_t width () const { return _width; }
    std::size_t height () const { return _height; }

    Pixbuf resized_canvas (std::size_t new_width, std::size_t new_height) const;
    void   resize_canvas  (std::size_t new_width, std::size_t new_height);

    Pixbuf resized_image (std::size_t new_width, std::size_t new_height) const;
    void   resize_image  (std::size_t new_width, std::size_t new_height);

    Pixbuf rotated (Vect center, GLdouble angle) const;
    void   rotate  (Vect center, GLdouble angle);

    Pixbuf mixed (Pixbuf other, GLdouble factor) const;
    void   mix   (Pixbuf other, GLdouble factor);

    Pixbuf gaussian_smoothed (double stdev) const;
    void   gaussian_smooth   (double stdev);

    Pixbuf sobel () const;
    void   add_sobel ();

    pixel  operator() (std::size_t x, std::size_t y) const;
    pixel& operator() (std::size_t x, std::size_t y);

    void desaturate ();
    void diff (Pixbuf other);

    void draw ();
};



#endif
#ifndef __PIXBUF__H__
#define __PIXBUF__H__

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

    bool range_check(std::size_t, std::size_t);

  public:
    Pixbuf () : Pixbuf(0, 0) {}
    Pixbuf (std::size_t width, std::size_t height)
      : _width{width}, _height{height}, _buffer{width * height} {}

    std::size_t width () const { return _width; }
    std::size_t height () const { return _height; }

    void set_size (std::size_t new_width, std::size_t new_height);

    pixel  operator() (std::size_t x, std::size_t y) const;
    pixel& operator() (std::size_t x, std::size_t y);

    void draw () const;
};



#endif
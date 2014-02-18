#ifndef __IMAGE_LOAD__H__
#define __IMAGE_LOAD__H__

#include <SDL.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  #define RMASK 0xff000000
  #define GMASK 0x00ff0000
  #define BMASK 0x0000ff00
  #define AMASK 0x000000ff
#else
  #define RMASK 0x000000ff
  #define GMASK 0x0000ff00
  #define BMASK 0x00ff0000
  #define AMASK 0xff000000
#endif

#include "Pixbuf.hpp"

#include <string>


std::string select_file_load ();
std::string select_file_save ();

Pixbuf image_load (std::string fname);
void image_save (std::string fname, Pixbuf image);

Pixbuf fb_load (std::size_t w, std::size_t h);

#endif
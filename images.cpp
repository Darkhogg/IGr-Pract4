#include "images.hpp"

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_image.h>

std::string select_file (std::string options);



std::string select_file_load () {
  return select_file(" 2>/dev/null");
}

std::string select_file_save () {
  return select_file("--save --filename=output.bmp 2>/dev/null");
}


std::string select_file (std::string options) {
  /* 1: Check for Zenity */
  FILE* pipe = popen(("zenity --file-selection " + options).c_str(), "r");
  if (pipe) {
    char buffer[128];
    std::string result = "";

    while (!feof(pipe)) {
      if (fgets(buffer, 128, pipe) != NULL) {
        result += buffer;
      }
    }

    int code = pclose(pipe);
    if (code == 0) {
      return result.substr(0, result.size() - 1);

    } else if (code == 256) {
      return "";
    }


    std::cout << "Zenity returned error " << code << std::endl;
  }

  /* 2: Check for stdin TTY */
  if (isatty(fileno(stdin))) {
    std::cout << "Write the full path to the image file: " << std::flush;

    std::string fname;
    std::cin >> fname;

    return fname;
  }

  /* 3: Return default image */
  return std::string{"./image.png"};
}


GLdouble select_factor () {
  /* 1: Check for Zenity */
  FILE* pipe = popen("zenity --scale --text=\"Select factor\\n  LEFT: Original image\\n  RIGHT: Loaded image\" --value=128 --hide-value --min-value=0 --max-value=255 2>/dev/null", "r");
  if (pipe) {
    char buffer[16];
    std::string result = "";

    while (!feof(pipe)) {
      if (fgets(buffer, 16, pipe) != NULL) {
        result += buffer;
      }
    }

    int code = pclose(pipe);
    if (code == 0) {
      std::istringstream iss(result);
      int i; iss >> i;
      return i / 255.f;

    } else if (code == 1) {
      return 0.5;
    }
  }

  /* 2: Check for stdin TTY */
  if (isatty(fileno(stdin))) {
    std::cout << "Write a factor (float between 0 and 1): " << std::flush;

    double factor;
    std::cin >> factor;

    return factor;
  }

  /* 3: Return default factor */
  return 0.5;
}


Pixbuf image_load (std::string fname) {
  SDL_Surface* surf = IMG_Load(fname.c_str());
  if (!surf) {
    return {};
  }


  Pixbuf buf {surf->w, surf->h};

  int bpp = surf->format->BytesPerPixel;
  for (std::size_t i = 0; i < surf->w; ++i) {
    for (std::size_t j = 0; j < surf->h; ++j) {
      Uint8* p = (Uint8*) surf->pixels + (int) j * surf->pitch + (int) i * bpp;

      Uint32 pixelColor = *(Uint32*)p;
      Uint8 red, green, blue, alpha;

      SDL_GetRGBA(pixelColor, surf->format, &red, &green, &blue, &alpha);
      buf(i, j) = {red, green, blue, alpha};
    }
  }

  SDL_FreeSurface(surf);
  return buf;
}

void image_save (std::string fname, Pixbuf image) {
  SDL_Surface* surf = SDL_CreateRGBSurface(SDL_SRCALPHA, image.width(), image.height(), 32,
    RMASK, GMASK, BMASK, AMASK);


  for (std::size_t i = 0; i < surf->w; ++i) {
    for (std::size_t j = 0; j < surf->h; ++j) {
      pixel px = image(i, j);
      int color = px.red << 0 | px.green << 8 | px.blue << 16 | px.alpha << 24;

      Uint8 * pixel = (Uint8*)surf->pixels;
      pixel += (j * surf->pitch) + (i * sizeof(Uint32));
      *((Uint32*)pixel) = color;
    }
  }

  SDL_SaveBMP(surf, fname.c_str());
  SDL_FreeSurface(surf);
}


Pixbuf fb_load (std::size_t w, std::size_t h) {
  unsigned char* buf = new unsigned char[3 * w * h];

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buf);

  Pixbuf pb {w, h};

  for (std::size_t x = 0; x < w; ++x) {
    for (std::size_t y = 0; y < h; ++y) {
      std::size_t idx = (x + y * w) * 3;

      pixel::color r = buf[idx];
      pixel::color g = buf[idx + 1];
      pixel::color b = buf[idx + 2];

      pb(x, h - y - 1) = {r, g, b};
    }
  }

  delete[] buf;

  return pb;
}
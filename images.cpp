#include "images.hpp"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include <SDL.h>
#include <SDL_image.h>

std::string select_file (std::string options);



std::string select_file_load () {
  return select_file("");
}

std::string select_file_save () {
  return select_file("--save --filename=output.bmp");
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

    } else if (code == 1) {
      return "";
    }
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
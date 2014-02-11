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
  return select_file("--save --filename=output.png");
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

  std::cout << "surface " << surf->w << " x " << surf->h << std::endl;
  std::cout << "buffer  " << buf.width() << " x " << buf.height() << std::endl;

  int bpp = surf->format->BytesPerPixel;
  for (std::size_t i = 0; i < surf->w; ++i) {
    for (std::size_t j = 0; j < surf->h; ++j) {
      Uint8* p = (Uint8*) surf->pixels + (int) j * surf->pitch + (int) i * bpp;

      Uint32 pixelColor = *(Uint32*)p;
      Uint8 red, green, blue, alpha;

      SDL_GetRGBA(pixelColor, surf->format, &red, &green, &blue, &alpha);
      buf(i, j) = {red, green, blue, alpha};

      //pixel px = buf(i, j);
      //std::cout << "(" << i << "," << j << ") :: " << px << std::endl;
    }
  }

  std::cout << " END" << std::endl;
  SDL_FreeSurface(surf);
  return buf;
}

void image_save (std::string fname, Pixbuf image) {

}
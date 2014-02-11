#ifndef __IMAGE_LOAD__H__
#define __IMAGE_LOAD__H__

#include "Pixbuf.hpp"

#include <string>


std::string select_file_load ();
std::string select_file_save ();

Pixbuf image_load (std::string fname);
void image_save (std::string fname, Pixbuf image);

#endif
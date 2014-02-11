#ifndef __IMAGE_SCENE__H__
#define __IMAGE_SCENE__H__

#include "skel/skel.h"
#include "images.hpp"

class ImageScene : public Scene {
  private:
    Pixbuf image;

  public:
    void onInitialize ();
    void onUpdate (float delta);
    void onDraw ();
    void onKeyDown (int code);
    void onMouseDown (int button);

};

#endif
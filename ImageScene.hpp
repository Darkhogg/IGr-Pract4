#ifndef __IMAGE_SCENE__H__
#define __IMAGE_SCENE__H__

#include "skel/skel.h"
#include "images.hpp"

#include "PtgTree.hpp"

class ImageScene : public Scene {
  private:
    Pixbuf image;
    Vect center;
    Vect::Component angle, angleUpdate; 
    std::string last_loaded;

    PtgTree tree;
    bool ptgmode;

  public:
    void onInitialize ();
    void onUpdate (float delta);
    void onDraw ();
    void onKeyDown (int code);
    void onMouseDown (int button);
    void onMouseUp (int button);

};

#endif
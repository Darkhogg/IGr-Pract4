#include "ImageScene.hpp"
#include <iostream>

void ImageScene::onInitialize () {

}

void ImageScene::onUpdate (float delta) {

}

void ImageScene::onDraw () {
  image.draw();
}

void ImageScene::onKeyDown (int code) {
  switch (code) {
    /* Load an image */
    case KEY_L: {
      auto fname = select_file_load();
      if (!fname.empty()) {
        image = image_load(fname);
      }
    }
    break;

    case KEY_S: {
      //if (image.width() > 0 && image.height() > 0) {
        auto fname = select_file_save();
        std::cout << fname << std::endl;
        if (!fname.empty()) {
          image_save(fname, image);
        }
      //}
    }
    break;

    case KEY_ESCAPE: {
      quit();
    }
    break;
  }
}

void ImageScene::onMouseDown (int button) {

}
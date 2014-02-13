#include "ImageScene.hpp"
#include <iostream>
#include <cmath>

void ImageScene::onInitialize () {

}

void ImageScene::onUpdate (float delta) {
  angle += delta * angleUpdate;
}

void ImageScene::onDraw () {
  auto cang = center.yaw();
  auto cmod = center.mod();
  cang += angle;

  auto tx = cmod * cos(cang);
  auto ty = cmod * sin(cang);

  glTranslated(center.x() - tx, center.y() - ty, 0.d);
  glRotated(angle * 180 / M_PI, 0.d, 0.d, 1.d);

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
      if (image.width() > 0 && image.height() > 0) {
        auto fname = select_file_save();
        std::cout << fname << std::endl;
        if (!fname.empty()) {
          image_save(fname, image);
        }
      }
    }
    break;

    case KEY_R: {
      double w = width();
      double h = height();
      std::cout << "Resize Canvas: " << w << " x " << h << std::endl;

      image.resize_canvas(w, h);
    }
    break;

    case KEY_F: {
      double w = width();
      double h = height();
      std::cout << "Resize Image: " << w << " x " << h << std::endl;

      image.resize_image(w, h);
    }
    break;

    case KEY_D: {
      image.desaturate();
    }
    break;

    case KEY_ESCAPE: {
      quit();
    }
    break;
  }
}

void ImageScene::onMouseDown (int button) {
  switch (button) {
    case MOUSE_LEFT: {
      angleUpdate = -1.0;
      center = getMouseWorldPosition();
    }
    break;

    case MOUSE_RIGHT: {
      angleUpdate = 1.0;
      center = getMouseWorldPosition();
    }
    break;
  }
}

void ImageScene::onMouseUp (int button) {
  std::cout << "Rotate Image: " << angle << " rad around" << center << std::endl;
  image.rotate(center, angle);

  angleUpdate = 0.0;
  angle = 0;
  center = Vect{0.d, 0.d, 0.d};
}
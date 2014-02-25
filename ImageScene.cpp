#include "ImageScene.hpp"
#include <iostream>
#include <cmath>

void ImageScene::onInitialize () {
  last_loaded = "";
}

void ImageScene::onUpdate (float delta) {
  if (!ptgmode) {
    angle += delta * angleUpdate;
  }
}

void ImageScene::onDraw () {
  if (ptgmode) {
    tree.draw();

  } else {
    auto cang = center.yaw();
    auto cmod = center.mod();
    cang += angle;

    auto tx = cmod * cos(cang);
    auto ty = cmod * sin(cang);

    glTranslated(center.x() - tx, center.y() - ty, 0.d);
    glRotated(angle * 180 / M_PI, 0.d, 0.d, 1.d);

    image.draw();
  }
}

void ImageScene::onKeyDown (int code) {
  switch (code) {
    /* Load an image */
    case KEY_L: {
      if (ptgmode) {
        ptgmode = false;

        std::cout << "Loading Tree into Image... " << std::flush;
        image = fb_load(width(), height());
        std::cout << "OK" << std::endl;

      } else {
        auto fname = select_file_load();
        if (!fname.empty()) {
          std::cout << "Loading Image '" << fname << "'... " << std::flush;
          image = image_load(fname);
          std::cout << "OK" << std::endl;
          last_loaded = fname;
        }
      }
    }
    break;

    case KEY_K: {
      if (!last_loaded.empty()) {
        std::cout << "Loading Image '" << last_loaded << "'... " << std::flush;
        image = image_load(last_loaded);
        std::cout << "OK" << std::endl;
      }
    }
    break;

    case KEY_S: {
      if (!ptgmode && image.width() > 0 && image.height() > 0) {
        auto fname = select_file_save();
        std::cout << fname << std::endl;
        if (!fname.empty()) {
          std::cout << "Saving Image '" << fname << "'... " << std::flush;
          image_save(fname, image);
          std::cout << "OK" << std::endl;
        }
      }
    }
    break;

    case KEY_R: {
      double w = width();
      double h = height();

      std::cout << "Resizing Canvas to " << w << " x " << h << "... " << std::flush;
      image.resize_canvas(w, h);
      std::cout << "OK" << std::endl;
    }
    break;

    case KEY_F: {
      double w = width();
      double h = height();

      std::cout << "Resizing Image to " << w << " x " << h << "... " << std::flush;
      image.resize_image(w, h);
      std::cout << "OK" << std::endl;
    }
    break;

    case KEY_P: {
      ptgmode = true;
    }
    break;

    case KEY_M: {
      auto fname = select_file_load();
      if (!fname.empty()) {
        auto factor = select_factor();

        std::cout << "Loading Image '" << fname << "'... " << std::flush;
        auto otherimg = image_load(fname);
        std::cout << "OK" << std::endl;

        std::cout << "Mixing Images with Factor " << factor << "... " << std::flush;
        image.mix(otherimg, factor);
        std::cout << "OK" << std::endl;
      }
    }
    break;

    case KEY_G: {
      std::cout << "Performing Gaussian Filter... " << std::flush;
      image.gaussian_smooth(GAUSS_STDEV);
      std::cout << "OK" << std::endl;
    }
    break;

    case KEY_Z: {
      auto fname = select_file_load();
      if (!fname.empty()) {
        std::cout << "Loading Image '" << fname << "'... " << std::flush;
        auto otherimg = image_load(fname);
        std::cout << "OK" << std::endl;

        std::cout << "Performing Difference... " << std::flush;
        image.diff(otherimg);
        std::cout << "OK" << std::endl;
      }
    }
    break;

    case KEY_X: {
      auto fname = select_file_load();
      if (!fname.empty()) {
        std::cout << "Loading Image '" << fname << "'... " << std::flush;
        auto otherimg = image_load(fname);
        std::cout << "OK" << std::endl;

        std::cout << "Desaturating Original Image... " << std::flush;
        image.desaturate();
        std::cout << "OK" << std::endl;


        std::cout << "Desaturating New Image... " << std::flush;
        otherimg.desaturate();
        std::cout << "OK" << std::endl;

        std::cout << "Performing Difference... " << std::flush;
        image.diff(otherimg);
        std::cout << "OK" << std::endl;
      }
    }
    break;

    case KEY_D: {
      std::cout << "Desaturating... " << std::flush;
      image.desaturate();
      std::cout << "OK" << std::endl;
    }
    break;

    case KEY_I: {
      std::cout << "Applying Sobel... " << std::flush;
      image = image.sobel();
      std::cout << "OK" << std::endl;
    }
    break;

    case KEY_O: {
      std::cout << "Applying Sobel Over Image... " << std::flush;
      image.add_sobel();
      std::cout << "OK" << std::endl;
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
  std::cout << "Rotating Image " << angle << " rad " << center << "... " << std::flush;
  image.rotate(center, angle);
  std::cout << "OK" << std::endl;

  angleUpdate = 0.0;
  angle = 0;
  center = Vect{0.d, 0.d, 0.d};
}
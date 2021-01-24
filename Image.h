#include "./matrix/include/led-matrix.h"
#include "./matrix/include/graphics.h"

#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>

#include <iostream> ///tmp


#pragma once


using std::min;
using std::max;
using std::cout; //tmp
using std::endl;

#define TERM_ERR  "\033[1;31m"
#define TERM_NORM "\033[0m"

using namespace rgb_matrix;

class DemoRunner {
protected:
  DemoRunner(Canvas *canvas) : canvas_(canvas) {}
  inline Canvas *canvas() { return canvas_; }

public:
  virtual ~DemoRunner() {}
  virtual void Run() = 0;

private:
  Canvas *const canvas_;
};

//Todo: Rename imagescroller to image
class ImageScroller : public DemoRunner {
public:
  // Scroll image with "scroll_jumps" pixels every "scroll_ms" milliseconds.
  // If "scroll_ms" is negative, don't do any scrolling.
    ImageScroller(RGBMatrix *m, int scroll_jumps, int scroll_ms = -30, FrameCanvas* off)
    : DemoRunner(m), scroll_jumps_(scroll_jumps),
        scroll_ms_(scroll_ms),
        horizontal_position_(0),
        matrix_(m) {
    //offscreen_ = matrix_->CreateFrameCanvas();
      offscreen_ = off;
    }

    // _very_ simplified. Can only read binary P6 PPM. Expects newlines in headers
    // Not really robust. Use at your own risk :)
    // This allows reload of an image while things are running, e.g. you can
    // live-update the content.
    bool LoadPPM(const char *filename);

    void Run() override;

    int getImageWidth();
    void setPosX(int pos);
    int getPostX();


private:
  struct Pixel {
    Pixel() : red(0), green(0), blue(0){}
    uint8_t red;
    uint8_t green;
    uint8_t blue;
  };

  struct Image {
    Image() : width(-1), height(-1), image(NULL) {}
    ~Image() { Delete(); }
    void Delete() { delete [] image; Reset(); }
    void Reset() { image = NULL; width = -1; height = -1; }
    inline bool IsValid() { return image && height > 0 && width > 0; }
    const Pixel &getPixel(int x, int y) {
      static Pixel black;
      if (x < 0 || x >= width || y < 0 || y >= height) return black;
      return image[x + width * y];
    }

    int width;
    int height;
    Pixel *image;
  };

  // Read line, skip comments.
  //TODO: Move these sometime
  char *ReadLine(FILE *f, char *buffer, size_t len) {
    char *result;
    do {
      result = fgets(buffer, len, f);
    } while (result != NULL && result[0] == '#');
    return result;
  }

  int curr_length;

  const int scroll_jumps_;
  const int scroll_ms_;

  // Current image is only manipulated in our thread.
  Image current_image_;

  // New image can be loaded from another thread, then taken over in main thread
  Mutex mutex_new_image_;
  Image new_image_;

  int32_t horizontal_position_;

  RGBMatrix* matrix_;
  FrameCanvas* offscreen_;
};









#include "Image.h"


bool ImageScroller::LoadPPM(const char *filename) {

    FILE *f = fopen(filename, "r");
    // check if file exists
    if (f == NULL && access(filename, F_OK) == -1) {
        fprintf(stderr, "File \"%s\" doesn't exist\n", filename);
        return false;
    }
    if (f == NULL) return false;
    char header_buf[256];
    const char *line = ReadLine(f, header_buf, sizeof(header_buf));
    #define EXIT_WITH_MSG(m) { fprintf(stderr, "%s: %s |%s", filename, m, line); \
        fclose(f); return false; }
    if (sscanf(line, "P6 ") == EOF)
        EXIT_WITH_MSG("Can only handle P6 as PPM type.");
    line = ReadLine(f, header_buf, sizeof(header_buf));
    int new_width, new_height;
    if (!line || sscanf(line, "%d %d ", &new_width, &new_height) != 2)
        EXIT_WITH_MSG("Width/height expected");
    int value;
    line = ReadLine(f, header_buf, sizeof(header_buf));
    if (!line || sscanf(line, "%d ", &value) != 1 || value != 255)
        EXIT_WITH_MSG("Only 255 for maxval allowed.");
    const size_t pixel_count = new_width * new_height;
    Pixel *new_image = new Pixel [ pixel_count ];
    assert(sizeof(Pixel) == 3);   // we make that assumption.
    if (fread(new_image, sizeof(Pixel), pixel_count, f) != pixel_count) {
        line = "";
        EXIT_WITH_MSG("Not enough pixels read.");
    }
    #undef EXIT_WITH_MSG
    fclose(f);
    fprintf(stderr, "Read image '%s' with %dx%d\n", filename,
            new_width, new_height);
    horizontal_position_ = -89;
    MutexLock l(&mutex_new_image_);
    new_image_.Delete();  // in case we reload faster than is picked up
    new_image_.image = new_image;
    new_image_.width = new_width;
    new_image_.height = new_height;
    return true;
}


void ImageScroller::Run() {

//Goal: Create a run function that only updates once everytime we call the function

const int screen_height = offscreen_->height();
const int screen_width = offscreen_->width();
    //cout << "Screen height x width: " << screen_height << " " << screen_width << endl;
    cout << "Image height and width: " << current_image_.height << " " << current_image_.width << endl;
    //Where to store image height and width?


    //Code for updating to new image if needed
    {
    MutexLock l(&mutex_new_image_);
    if (new_image_.IsValid()) {
        current_image_.Delete();
        current_image_ = new_image_;
        new_image_.Reset();
    }
    }


    if (!current_image_.IsValid()) {
    usleep(100 * 1000);
    //continue;
    }

    cout << "horizontal position: " << horizontal_position_ % current_image_.width << endl;

    for (int x = 0; x < screen_width; ++x) {
    for (int y = 0; y < screen_height; ++y) {
        //cout << ""
        //const Pixel &p = current_image_.getPixel((horizontal_position_ + x) % current_image_.width, y);
        const Pixel &p = current_image_.getPixel((horizontal_position_ + x), y);

        offscreen_->SetPixel(x, y, p.red, p.green, p.blue);
    }
    }
    offscreen_ = matrix_->SwapOnVSync(offscreen_);
    horizontal_position_ += scroll_jumps_;
    //if (horizontal_position_ < 0) horizontal_position_ = current_image_.width;
    
    //Normally the function would just sleep the designated amount of time, and 
    //then restart the loop once the time has passed...instead we want to dynamically
    //call to update the image on demand
    /*
    if (scroll_ms_ <= 0) {
    // No scrolling. We don't need the image anymore.
    current_image_.Delete();
    } else {
    usleep(scroll_ms_ * 1000);
    } */

}
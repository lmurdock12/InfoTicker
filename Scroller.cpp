


#include <iostream>

#include "./matrix/include/led-matrix.h"
#include "./matrix/include/graphics.h"

#include "Item.h"
#include "Image.h"

#include "StockManager.h"


using namespace std;
using namespace rgb_matrix;

static void add_micros(struct timespec *accumulator, long micros) {
  const long billion = 1000000000;
  const int64_t nanos = (int64_t) micros * 1000;
  accumulator->tv_sec += nanos / billion;
  accumulator->tv_nsec += nanos % billion;
  while (accumulator->tv_nsec > billion) {
    accumulator->tv_nsec -= billion;
    accumulator->tv_sec += 1;
  }
}
static bool FullSaturation(const Color &c) {
  return (c.r == 0 || c.r == 255)
    && (c.g == 0 || c.g == 255)
    && (c.b == 0 || c.b == 255);
}

int main() {


    cout << "hello world" << endl;

    RGBMatrix::Options matrix_options;
    rgb_matrix::RuntimeOptions runtime_opt;

        // These are the defaults when no command-line flags are given.
    matrix_options.rows = 32;
    matrix_options.chain_length = 1;
    matrix_options.parallel = 1;
    matrix_options.hardware_mapping = "adafruit-hat";
    matrix_options.cols = 64;

    //Disregarding the demo parameter for now..
    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
    if (matrix == NULL)
    return 1;

    printf("Size: %dx%d. Hardware gpio mapping: %s\n",
            matrix->width(), matrix->height(), matrix_options.hardware_mapping);

    


    /*********************************************************/
    //Image Initialization
    const char* img = "ford-32-2.ppm";
    ImageScroller *scroller = new ImageScroller(matrix, 1,50);
    scroller->LoadPPM(img);


    /*********************************************/
    //Text initialization



    const int board_size = matrix_options.chain_length * matrix_options.cols;
    
    RGBMatrix *canvas = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt); 

    Color color(255, 255, 255);
    Color bg_color(0, 0, 0);
    Color outline_color(0,0,0);

  //Todo: see what this is for
  const bool all_extreme_colors = (matrix_options.brightness == 100)
    && FullSaturation(color)
    && FullSaturation(bg_color)
    && FullSaturation(outline_color);
  //if (all_extreme_colors)
    //canvas->SetPWMBits(1);

    const char* bdf_font_file = "./matrix/fonts/8x13.bdf"; //Hardcoding the font type for now
    rgb_matrix::Font font;
    if (!font.LoadFont(bdf_font_file)) {
    fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file);
    return 1;
    } else {
        cout << "loaded" << endl;
    }


    int letter_spacing =0;
  const int x_default_start = board_size + 5;
  float speed = 7.0f;
    int x_orig = x_default_start - 6;

  // Create a new canvas to be used with led_matrix_swap_on_vsync
  FrameCanvas *offscreen_canvas = canvas->CreateFrameCanvas();

  int delay_speed_usec = 1000000;
  if (speed > 0) {
    delay_speed_usec = 1000000 / speed / font.CharacterWidth('W');
  } 
  struct timespec next_frame = {0, 0};


    Item* secondItem = new Item(64,10,"TSLA  ", letter_spacing, &font,color,64);

    StockManager* mainScroller = new StockManager(scroller,secondItem);



    /*********************************************/


    offscreen_canvas->Fill(bg_color.r, bg_color.g, bg_color.b);
    mainScroller->resetLocations();

    for(int i=0; i<150;i++) {
        ///Make sure we are using one canvas for drawing both image and text?????

        offscreen_canvas->Fill(bg_color.r, bg_color.g, bg_color.b); 
        //secondItem->drawItem(offscreen_canvas,board_size);
        mainScroller->updateLocations(offscreen_canvas,board_size);
        //usleep(100* 1000);


        // Make sure render-time delays are not influencing scroll-time
        
        if (speed > 0) {
            if (next_frame.tv_sec == 0) {
                // First time. Start timer, but don't wait.
                clock_gettime(CLOCK_MONOTONIC, &next_frame);
            } else {
                add_micros(&next_frame, delay_speed_usec);
                clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_frame, NULL);
            }
        }

        // Swap the offscreen_canvas with canvas on vsync, avoids flickering
        offscreen_canvas = canvas->SwapOnVSync(offscreen_canvas);
        if (speed <= 0) pause();  // Nothing to scroll.

    } 
}
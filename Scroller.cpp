#include <iostream>

#include "./matrix/include/led-matrix.h"
#include "./matrix/include/graphics.h"

#include <string>

#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <tuple>
#include <iostream>
#include <queue>
#include <thread>


using namespace std;
using namespace rgb_matrix;


#include "StockManager.h"

#include "Item.h"


volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}

static int usage(const char *progname) {
  fprintf(stderr, "usage: %s [options] <text>\n", progname);
  fprintf(stderr, "Takes text and scrolls it with speed -s\n");
  fprintf(stderr, "Options:\n");
  fprintf(stderr,
          "\t-s <speed>        : Approximate letters per second. "
          "(Zero for no scrolling)\n"
          "\t-l <loop-count>   : Number of loops through the text. "
          "-1 for endless (default)\n"
          "\t-f <font-file>    : Path to *.bdf-font to be used.\n"
          "\t-x <x-origin>     : Shift X-Origin of displaying text (Default: 0)\n"
          "\t-y <y-origin>     : Shift Y-Origin of displaying text (Default: 0)\n"
          "\t-t <track-spacing>: Spacing pixels between letters (Default: 0)\n"
          "\n"
          "\t-C <r,g,b>        : Text Color. Default 255,255,255 (white)\n"
          "\t-B <r,g,b>        : Background-Color. Default 0,0,0\n"
          "\t-O <r,g,b>        : Outline-Color, e.g. to increase contrast.\n"
          );
  fprintf(stderr, "\nGeneral LED matrix options:\n");
  rgb_matrix::PrintMatrixFlags(stderr);
  return 1;
}

static bool parseColor(Color *c, const char *str) {
  return sscanf(str, "%hhu,%hhu,%hhu", &c->r, &c->g, &c->b) == 3;
}

static bool FullSaturation(const Color &c) {
  return (c.r == 0 || c.r == 255)
    && (c.g == 0 || c.g == 255)
    && (c.b == 0 || c.b == 255);
}

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


int main(int argc, char *argv[]) {

  RGBMatrix::Options matrix_options;
  rgb_matrix::RuntimeOptions runtime_opt;
  if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv,
                                         &matrix_options, &runtime_opt)) {
    return usage(argv[0]);
  }

  Color color(255, 255, 255);
  Color bg_color(0, 0, 0);
  Color outline_color(0,0,0);

  Color negative_color(255,0,0);
  
  bool with_outline = false;

  const char *bdf_font_file = NULL;
  std::string line;
  const int board_size = matrix_options.chain_length * matrix_options.cols;
  /* x_origin is set by default just right of the screen */
  const int x_default_start = board_size + 5;
  int x_orig = x_default_start - 6;
  int y_orig = 0;
  int letter_spacing = 0;
  float speed = 7.0f;
  int loops = -1;

  int opt;
  while ((opt = getopt(argc, argv, "x:y:f:C:B:O:t:s:l:")) != -1) {
    switch (opt) {
    case 's': speed = atof(optarg); break;
    case 'l': loops = atoi(optarg); break;
    case 'x': x_orig = atoi(optarg); break;
    case 'y': y_orig = atoi(optarg); break;
    case 'f': bdf_font_file = strdup(optarg); break;
    case 't': letter_spacing = atoi(optarg); break;
    case 'C':
      if (!parseColor(&color, optarg)) {
        fprintf(stderr, "Invalid color spec: %s\n", optarg);
        return usage(argv[0]);
      }
      break;
    case 'B':
      if (!parseColor(&bg_color, optarg)) {
        fprintf(stderr, "Invalid background color spec: %s\n", optarg);
        return usage(argv[0]);
      }
      break;
    case 'O':
      if (!parseColor(&outline_color, optarg)) {
        fprintf(stderr, "Invalid outline color spec: %s\n", optarg);
        return usage(argv[0]);
      }
      with_outline = true;
      break;
    default:
      return usage(argv[0]);
    }
  }

  for (int i = optind; i < argc; ++i) {
    line.append(argv[i]).append(" ");
  }

  if (line.empty()) {
    fprintf(stderr, "Add the text you want to print on the command-line.\n");
    return usage(argv[0]);
  }

  if (bdf_font_file == NULL) {
    fprintf(stderr, "Need to specify BDF font-file with -f\n");
    return usage(argv[0]);
  }

  /*
   * Load font. This needs to be a filename with a bdf bitmap font.
   */
  //cout << bdf_font_file << endl;
  rgb_matrix::Font font;
  if (!font.LoadFont(bdf_font_file)) {
    fprintf(stderr, "Couldn't load font '%s'\n", bdf_font_file);
    return 1;
  }

  /*
   * If we want an outline around the font, we create a new font with
   * the original font as a template that is just an outline font.
   */
  rgb_matrix::Font *outline_font = NULL;
  if (with_outline) {
    outline_font = font.CreateOutlineFont();
  }

  RGBMatrix *canvas = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
  if (canvas == NULL)
    return 1;

  //Todo: see what this is for
  const bool all_extreme_colors = (matrix_options.brightness == 100)
    && FullSaturation(color)
    && FullSaturation(bg_color)
    && FullSaturation(outline_color);
  //if (all_extreme_colors)
    //canvas->SetPWMBits(1); ///Disabling this also halved the refresh rate...why?

  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  printf("CTRL-C for exit.\n");

  // Create a new canvas to be used with led_matrix_swap_on_vsync
  FrameCanvas *offscreen_canvas = canvas->CreateFrameCanvas();

  int delay_speed_usec = 1000000;
  if (speed > 0) {
    delay_speed_usec = 1000000 / speed / font.CharacterWidth('W');
  } else if (x_orig == x_default_start) {
    // There would be no scrolling, so text would never appear. Move to front.
    x_orig = with_outline ? 1 : 0;
  }

  int x = x_orig;
  int y = 30;//y_orig;

  Item* secondItem = new Item(x,15,"FORD",letter_spacing,&font,color, x_orig);
  Item* firstItem = new Item(x,15,"TSLA",letter_spacing,&font,color,x_orig);
  // Item* third = new Item(x,y,"MRVL  ",letter_spacing,&font,color,x_orig);
  // Item* fourth = new Item(x,y,"MSFT  ",letter_spacing,&font,color,x_orig);
  // Item* fifth = new Item(x,y,"AAPL ",letter_spacing,&font,color,x_orig);

  Item* price = new Item(x,30,"11.51",letter_spacing,&font,negative_color,x_orig);
  Item* price2 = new Item(x,30,"585.30",letter_spacing,&font,negative_color,x_orig);


  // queue<Item*> readyItems;

  // vector<Item*> currentItems;
  // vector<Item*>::iterator it;

  // readyItems.push(secondItem);
  // currentItems.push_back(firstItem);
  // readyItems.push(third);
  // readyItems.push(fourth);
  // readyItems.push(fifth);
  //currentItems.push_back(secondItem);
  
  //Using simplified image class right now...could probably be improved in the future
  //with the more advanced class 
  const char* img = "./images/logos/ford-32-2.ppm";
  ImageScroller *scroller = new ImageScroller(canvas,1,50);
  scroller->LoadPPM(img);

  const char* img2 = "tesla-final-32.ppm";
  ImageScroller *scroller2 = new ImageScroller(canvas,1,50);
  scroller2->LoadPPM(img2);
  
  
  const char* arrow_img = "images/utilities/Green-Up-Arrow-32.ppm";
  ImageScroller *arrow_scroller = new ImageScroller(canvas,1,50);
  arrow_scroller->LoadPPM(arrow_img);

  ImageScroller *arrow_scroller2 = new ImageScroller(canvas,1,50);
  arrow_scroller2->LoadPPM(arrow_img);
  //should not need to pass in the up arrow and down arrow...should automatically load those up when
  //initializing a new stockManager class
  
  //WRITE IN NOTES:
  //images must be scaled to 32 bit p6 (raw) ppm

  StockManager* mainScroller = new StockManager(scroller,secondItem,price,arrow_scroller);
  mainScroller->resetLocations();
  mainScroller->updateLocations(offscreen_canvas,board_size);
  //offscreen_canvas = canvas->SwapOnVSync(offscreen_canvas);
  mainScroller->resetLocations();

  StockManager* mainScroller2 = new StockManager(scroller2,firstItem,price2,arrow_scroller2);
  mainScroller2->resetLocations();
  mainScroller2->updateLocations(offscreen_canvas,board_size);
  //offscreen_canvas = canvas->SwapOnVSync(offscreen_canvas);
  mainScroller2->resetLocations();

  offscreen_canvas->Clear();



  queue<StockManager*> readyItems;
  vector<StockManager*> currItems;
  vector<StockManager*>::iterator it;
  currItems.push_back(mainScroller);
  //currItems.push_back(mainScroller2);
  readyItems.push(mainScroller2);


  int length = 0;
  struct timespec next_frame = {0, 0};




//Multi stock scroller:

//Use two queues to keep track of the items we are currently renering, and what we are rendering next
//Inside the program loop:
  //1. Loop through all the stocks in CurrItems
    //a. Render a given StockManager
    //b. check if the end is >= 0:
      //reset and add to readyItems if so
  //If we removed a stock from currItems, add the next stock from ready to currItems
  bool addNew;
  //cout << "Board size: " << board_size << endl;
  /*
  while (!interrupt_received && loops != 0) {

    offscreen_canvas->Clear();
    offscreen_canvas->Fill(bg_color.r, bg_color.g, bg_color.b);
    //mainScroller->updateLocations(offscreen_canvas,board_size);
    //usleep(50*1000);

    addNew = false;
    for(it = currItems.begin(); it!=currItems.end();) {
      int currInd = std::distance(currItems.begin(),it);
      //cout << "pos end: " << (*it)->getPosEnd() << endl;
      bool reset = (*it)->updateLocations(offscreen_canvas,board_size);
      if(currInd == 0 && reset) {
        //Location was reset..remove from curr items
        ////cout << "REMOVING ITEM" << endl;
        readyItems.push(*it);
        currItems.erase(it);
        addNew = true;

        //Special case if only 1 value in queue
        if (currItems.size() == 0) {
          if(!readyItems.empty()) {
            currItems.push_back(readyItems.front());
            readyItems.pop();
          }
          break;
        }
        continue;

      }

      if (currInd == (currItems.size()-1) && (*it)->getPosEnd() >= -58) {
        if(!readyItems.empty()) {
          currItems.push_back(readyItems.front());
          readyItems.pop();
          break;
        }
      }
      ////cout << "SECOND AND BEYOND" << endl;

      it++;
    }

    //mainScroller->updateLocations(offscreen_canvas,board_size);

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
  */




  std::cout << "Exiting Scroller...goodbye" << std::endl;

  // Finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;
  return 0;
}

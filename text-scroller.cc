// -*- mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; -*-
// Copyright (C) 2015 Henner Zeller <h.zeller@acm.org>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation version 2.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://gnu.org/licenses/gpl-2.0.txt>

#include "led-matrix.h"
#include "graphics.h"

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

using namespace std;
using namespace rgb_matrix;


class Item {

public:

    string name = "";
    int x = -1;
    int y = -1;
    int length = -1;
    int letterSpacing = 0;
    int x_orig = -1;


    bool leftBound = false;
    bool rightBound = false;

    Color currColor; //Holds the current font color
    Font* fontPtr; //Holds the current font;

    Item();
    Item(int x, int y, string defaultName, int letterSpacing,Font* font,
        Color color, int x_start);
    //TODO:add background color initliazation
    void drawItem(rgb_matrix::Canvas *c, int rightBoundry);



private:
    void setName(string newName);
    //TODO: Update color method
    //TODO: Implement background color

};

Item::Item(int x_cord, int y_cord, string defaultName, int letterSpacing,Font* font,
        Color color, int x_start) {

    x = x_cord;
    x_orig = x_start;
    y = y_cord;
    name = defaultName;
    letterSpacing = letterSpacing;
    fontPtr = font;
    currColor = color;

}

Item::Item() {
    name = "default";
}

void Item::setName(string newName) {
    name = newName;
}

void Item::drawItem(rgb_matrix::Canvas *c, int rightBoundry) {

        //Todo: add outline font drawing

        ////cout << "Current X: " << x << " length: " << length << " end edge: " << x + length << endl;
        length = rgb_matrix::DrawText(c,*fontPtr,x,y,currColor,
                    NULL,name.c_str(),letterSpacing);

        //currColor.r++;
        //currColor.g--;
        --x;



        //assume speed > 0 (original if loop check)
        //not doing any loop checking either (used if we want to loop the text a specific amount of times)
        if(x+length<0) {
          leftBound = true;
          x = x_orig;
        }
        if(x+length < rightBoundry) {
          rightBound = true;
        }

    }



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

  const bool all_extreme_colors = (matrix_options.brightness == 100)
    && FullSaturation(color)
    && FullSaturation(bg_color)
    && FullSaturation(outline_color);
  if (all_extreme_colors)
    canvas->SetPWMBits(1);

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
  int y = 10;//y_orig;



  Item* secondItem = new Item(x,y,"TSLA  ",letter_spacing,&font,color, x_orig);
  Item* firstItem = new Item(x,y,"NVDIA  ",letter_spacing,&font,color,x_orig);
  Item* third = new Item(x,y,"MRVL  ",letter_spacing,&font,color,x_orig);
  Item* fourth = new Item(x,y,"MSFT  ",letter_spacing,&font,color,x_orig);
  Item* fifth = new Item(x,y,"AAPL ",letter_spacing,&font,color,x_orig);

  queue<Item*> readyItems;

  vector<Item*> currentItems;
  vector<Item*>::iterator it;

  readyItems.push(secondItem);
  currentItems.push_back(firstItem);
  readyItems.push(third);
  readyItems.push(fourth);
  readyItems.push(fifth);
  //currentItems.push_back(secondItem);
  

  int length = 0;
  struct timespec next_frame = {0, 0};

  while (!interrupt_received && loops != 0) {
    offscreen_canvas->Fill(bg_color.r, bg_color.g, bg_color.b);
    

    //for(int i=0; i<currentItems.size();i++) {
      //currentItems[i]->drawItem(offscreen_canvas,board_size);
    //}


    /*if (outline_font) {
      // The outline font, we need to write with a negative (-2) text-spacing,
      // as we want to have the same letter pitch as the regular text that
      // we then write on top.
      rgb_matrix::DrawText(offscreen_canvas, *outline_font,
                           x - 1, y + font.baseline(),
                           outline_color, NULL,
                           line.c_str(), letter_spacing - 2);
    }*/
    
    ////cout << "size: "  << currentItems.size() << "curr index: " << currInd << endl;
    //cout << " curr item size: "  << currentItems.size() << "ready item size: " << readyItems.size() << endl;
    for(it = currentItems.begin(); it != currentItems.end();) {

      int currInd = std::distance(currentItems.begin(),it);

      (*it)->drawItem(offscreen_canvas,board_size);

      //Check first item
      if(currInd==0 && (*it)->leftBound) {
        //Remove from current, reset it, and add to queue
        readyItems.push(*it); //TODO:reset the item here
        (*it)->x =(*it)->x_orig;//Todo move
        (*it)->rightBound = false;
        (*it)->leftBound = false;
        currentItems.erase(it);
        //cout << "READY SIZE: " << readyItems.size() << " " << currentItems.size() << endl;

        continue;
        //dont increment it here:

      }

      if(currInd==(currentItems.size()-1) && (*it)->rightBound) {
        ////cout << "yes" << endl;
        if(!readyItems.empty()) {
          //cout << "no" << endl;
          currentItems.push_back(readyItems.front());
          readyItems.pop();
          break;
        }
      }


      it++;

    }

    //check if less than or equal to last edge...if so increment lastRender
    //check if less then or equal to first edge...if so increment firstRender
    //if firstRender == last element...reset...

    
    /*
    length = rgb_matrix::DrawText(offscreen_canvas, font,
                                  x, y + font.baseline(),
                                  color, NULL,
                                  firstItem->name.c_str(), letter_spacing);
    length = rgb_matrix::DrawText(offscreen_canvas, font,
                                  x-60, y + font.baseline(),
                                  color, NULL,
                                  secondItem->name.c_str(), letter_spacing);
  */

    //--x;
    if (speed > 0 && x + length < 0) {  // moved all the way left out of frame.
      //x = x_orig;
      if (loops > 0) --loops;
    }

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

  // Finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;

  return 0;
}

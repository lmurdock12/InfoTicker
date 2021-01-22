
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
#include "Image.h"

using std::min;
using std::max;
using std::cout; //tmp
using std::endl;

#define TERM_ERR  "\033[1;31m"
#define TERM_NORM "\033[0m"

using namespace rgb_matrix;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
  interrupt_received = true;
}



static int usage(const char *progname) {
  fprintf(stderr, "usage: %s <options> -D <demo-nr> [optional parameter]\n",
          progname);
  fprintf(stderr, "Options:\n");
  fprintf(stderr,
          "\t-D <demo-nr>              : Always needs to be set\n"
          );


  rgb_matrix::PrintMatrixFlags(stderr);

  fprintf(stderr, "Demos, choosen with -D\n");
  fprintf(stderr, "\t0  - some rotating square\n"
          "\t1  - forward scrolling an image (-m <scroll-ms>)\n"
          "\t2  - backward scrolling an image (-m <scroll-ms>)\n"
          "\t3  - test image: a square\n"
          "\t4  - Pulsing color\n"
          "\t5  - Grayscale Block\n"
          "\t6  - Abelian sandpile model (-m <time-step-ms>)\n"
          "\t7  - Conway's game of life (-m <time-step-ms>)\n"
          "\t8  - Langton's ant (-m <time-step-ms>)\n"
          "\t9  - Volume bars (-m <time-step-ms>)\n"
          "\t10 - Evolution of color (-m <time-step-ms>)\n"
          "\t11 - Brightness pulse generator\n");
  fprintf(stderr, "Example:\n\t%s -D 1 runtext.ppm\n"
          "Scrolls the runtext until Ctrl-C is pressed\n", progname);
  return 1;
}

int main(int argc, char *argv[]) {


    int demo = -1;
    int scroll_ms = -30;

    const char *demo_parameter = NULL;
    RGBMatrix::Options matrix_options;
    rgb_matrix::RuntimeOptions runtime_opt;


    // These are the defaults when no command-line flags are given.
    matrix_options.rows = 32;
    matrix_options.chain_length = 1;
    matrix_options.parallel = 1;

    // First things first: extract the command line flags that contain
    // relevant matrix options.
    if (!ParseOptionsFromFlags(&argc, &argv, &matrix_options, &runtime_opt)) {
    return usage(argv[0]);
    }

    int opt;
    while ((opt = getopt(argc, argv, "dD:r:P:c:p:b:m:LR:")) != -1) {
    switch (opt) {
    case 'D':
        demo = atoi(optarg);
        break;

    case 'm':
        scroll_ms = atoi(optarg);
        break;

    default: /* '?' */
        return usage(argv[0]);
    }
    }

    if (optind < argc) {
    //The specifc parameter with no usage goes here

    demo_parameter = argv[optind];
    cout << demo_parameter << endl;
    }

    if (demo < 0) {
    fprintf(stderr, TERM_ERR "Expected required option -D <demo>\n" TERM_NORM);
    return usage(argv[0]);
    }


    //Disregarding the demo parameter for now..
    RGBMatrix *matrix = RGBMatrix::CreateFromOptions(matrix_options, runtime_opt);
    if (matrix == NULL)
    return 1;

    printf("Size: %dx%d. Hardware gpio mapping: %s\n",
            matrix->width(), matrix->height(), matrix_options.hardware_mapping);

    Canvas *canvas = matrix;

    // The DemoRunner objects are filling
    // the matrix continuously.
    DemoRunner *demo_runner = NULL;


    if (demo_parameter) {
        ImageScroller *scroller = new ImageScroller(matrix,
                                                    demo == 1 ? 1 : -1,
                                                    scroll_ms);
        cout << demo_parameter << endl;
        if (!scroller->LoadPPM(demo_parameter))
        return 1;
        demo_runner = scroller;
    } else {
        fprintf(stderr, "Demo %d Requires PPM image as parameter\n", demo);
        return 1;
    }

    if (demo_runner == NULL)
    return usage(argv[0]);

    // Set up an interrupt handler to be able to stop animations while they go
    // on. Each demo tests for while (!interrupt_received) {},
    // so they exit as soon as they get a signal.
    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    printf("Press <CTRL-C> to exit and reset LEDs\n");

    // Now, run our particular demo; it will exit when it sees interrupt_received.
    demo_runner->Run();
    int scroll_ms_ = 50;

    for(int i=0; i<100;i++) {
      usleep(scroll_ms_ * 1000);
      demo_runner->Run();
    }

    
    delete demo_runner;
    delete canvas;

    printf("Received CTRL-C. Exiting.\n");
    return 0;


}
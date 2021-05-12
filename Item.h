
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

using namespace std;
using namespace rgb_matrix;

#pragma once

class Item {

public:

    Item();
    Item(int x, int y, string defaultName, int letterSpacing,Font* font,
        Color color, int x_start);
    //TODO:add background color initliazation
    void drawItem(rgb_matrix::Canvas *c, int rightBoundry);

    void setName(string newName);
    //TODO: Update color method
    //TODO: Implement background color

    int getWidth();

    void setPosX(int pos);
    int getPosX();

    int getPosEnd();

    void setPosY(int pos);
    int getPosY();

private:


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


};
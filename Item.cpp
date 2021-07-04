

#include "Item.h"
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
#include <queue> //TODO: Do i need these includes?

using namespace std;
using namespace rgb_matrix;

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




int Item::getWidth() {
  return length;
}

void Item::setPosX(int pos) {
  x = pos;
  x_orig = pos;
}

//TODO: change to getPosStart
int Item::getPosX() {
  return x;
}

int Item::getPosEnd() {
  return x + length;
}

int Item::getLength() {
  return length;
}

void Item::setPosY(int pos) {
  y = pos;
}

int Item::getPosY() {
  return y;
}
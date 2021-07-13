#include "./matrix/include/led-matrix.h"
#include "./matrix/include/graphics.h"

#include "Item.h" //whhy cant I have these ??
#include "Image.h"

using namespace rgb_matrix;


class StockManager {


public:


    //StockManager(ImageScroller* stock,Item* ticker, ImageScroller* status = nullptr,
               // Item* price = nullptr, Item* diff = nullptr); 
    //StockManager(ImageScroller* stock,Item* ticker, Item* price);
    StockManager(RGBMatrix* canv, Font* txtFont, ImageScroller* stock,string symbol,int currPrice,int startPrice);
    void resetLocations();
    
    bool updateLocations(rgb_matrix::Canvas *c, int rightBoundry); //refractor arguments at some point
    int getPosEnd();
private:

    Color red_color = Color(255,255,255);
    Color green_color = Color(0,128,0);
    Color white_color = Color(255,255,255);

    int currPrice = 0;
    int startPrice = 0;
    int currDiff = 0;

    ImageScroller* stock = nullptr;
    ImageScroller* arrow = nullptr;
    ImageScroller* status = nullptr;
    Item* ticker = nullptr;
    Item* price = nullptr;
    Item* diff = nullptr;

    ImageScroller* upArrow = nullptr;
    ImageScroller* downArrow = nullptr;


};
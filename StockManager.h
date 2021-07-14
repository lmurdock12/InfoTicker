#include "./matrix/include/led-matrix.h"
#include "./matrix/include/graphics.h"

#include "Item.h" //whhy cant I have these ??
#include "Image.h"
#include <sstream>
#include <iomanip>



using namespace rgb_matrix;


class StockManager {


public:


    //StockManager(ImageScroller* stock,Item* ticker, ImageScroller* status = nullptr,
               // Item* price = nullptr, Item* diff = nullptr); 
    //StockManager(ImageScroller* stock,Item* ticker, Item* price);
    StockManager(RGBMatrix* canv, Font* txtFont, ImageScroller* stock,string symbol,double currPrice,double startPrice);
    void resetLocations();
    
    bool updateLocations(rgb_matrix::Canvas *c, int rightBoundry); //refractor arguments at some point
    int getPosEnd();
private:

    Color red_color = Color(255,0,0);
    Color green_color = Color(0,128,0);
    Color white_color = Color(255,255,255);

    float currPrice = 0;
    float startPrice = 0;
    float currDiff = 0;

    ImageScroller* stock = nullptr;
    ImageScroller* arrow = nullptr;
    ImageScroller* status = nullptr;
    Item* ticker = nullptr;
    Item* price = nullptr;
    Item* diff = nullptr;

    ImageScroller* upArrow = nullptr;
    ImageScroller* downArrow = nullptr;


};
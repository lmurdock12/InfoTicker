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
    StockManager(ImageScroller* stock, Item* ticker, Item* price, ImageScroller* arrow);
    void resetLocations();
    
    bool updateLocations(rgb_matrix::Canvas *c, int rightBoundry); //refractor arguments at some point
    int getPosEnd();
private:


    ImageScroller* stock = nullptr;
    ImageScroller* arrow = nullptr;
    ImageScroller* status = nullptr;
    Item* ticker = nullptr;
    Item* price = nullptr;
    Item* diff = nullptr;


};
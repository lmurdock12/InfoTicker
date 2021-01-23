

#include "StockManager.h"

/*


//image and txt classes need a grab length attribute
//move image and txt attributes to the private section


//Infomanager(img_1 pointer, img_2 pointer, txt_1 ptr
            txt_2 ptr, txt_3 ptr)

//Infomanager class that resets the items to the appropriate coords

s


*/


/*StockManager::StockManager(ImageScroller* stock,Item* ticker, ImageScroller* status = nullptr,
                            Item* price = nullptr, Item* diff = nullptr) {



    this->stock = stock;
    this->status = status;
    this->ticker = ticker;
    this->price = price;
    this->diff = diff;

}*/

StockManager::StockManager(ImageScroller* stock,Item* ticker) {

    this->stock = stock;
    this->ticker = ticker;

}


void StockManager::resetLocations() {

    int stockWidth = stock->getImageWidth();
    cout << "width: " << stockWidth << endl;
    //Set stock starting point
    cout << "1." << endl;
    stock->setPosX(-stockWidth);
    cout << "2." << endl;
    //Set Text starting point
    //TODO: update so not static 64 (i.e use rightBoundry)
    ticker->setPosX(64+stockWidth);

}


void StockManager::updateLocations(rgb_matrix::Canvas *c, int rightBoundry) {

    stock->Run();
    
    ticker->drawItem(c,rightBoundry);

}

//TODO: Create a function that gets the initial values of everything and sets them to the apporpriate spot
//TODO: Create a function that manages the scrolling. 
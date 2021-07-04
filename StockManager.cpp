

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

StockManager::StockManager(ImageScroller* stock,Item* ticker,Item* price,ImageScroller* arrow) {

    this->stock = stock;
    this->ticker = ticker;
    this->price = price;
    this->arrow = arrow;

}


void StockManager::resetLocations() {

    int stockWidth = stock->getImageWidth();
    cout << "width: " << stockWidth << endl;
    //Set stock starting point

    //TEMP:
    //arrow->setPosX(-64);
    //stockWidth = stock->getImageWidth();

    //stock->setPosX(-stockWidth);
    stock->setPosX(-64);
    //Set Text starting point
    //TODO: update so not static 64 (i.e use rightBoundry)
    //ticker->setPosX(64+stockWidth+30);
    ticker->setPosX(64 + stockWidth + 5);//+stockWidth+10);
    price->setPosX(64 + stockWidth + 5);

    //arrow->setPosX(-64 + -1*(stockWidth + ticker->getPosEnd()));
    arrow->setPosX(-1*ticker->getPosEnd()  - 10);

    cout << "stock: " << stock->getPostX() << " ticker: " << price->getPosX() << ", end: " << price->getPosEnd() << endl;
    cout << price->getLength() << endl;
    cout << arrow->getPostX() << endl;

}


void StockManager::updateLocations(rgb_matrix::Canvas *c, int rightBoundry) {

    c->Clear();
    
    stock->Run(c);
    arrow->Run(c);

    cout << arrow->getPostX() - arrow->getImageWidth() << endl;
    ticker->drawItem(c,rightBoundry);
    price->drawItem(c,rightBoundry);
    //arrow->Run(c);

    //INFO: Images are negative based going right
    //      Words are positive based going right
    
    //cout << price->getPosEnd() << endl;
    if (arrow->getPosEnd() >= 0) {
        resetLocations();
    }


}

//TODO: Create a function that gets the initial values of everything and sets them to the apporpriate spot
//TODO: Create a function that manages the scrolling. 
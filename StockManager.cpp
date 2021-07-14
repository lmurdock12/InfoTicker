

#include "StockManager.h"

/*


//image and txt classes need a grab length attribute
//move image and txt attributes to the private section


//Infomanager(img_1 pointer, img_2 pointer, txt_1 ptr
            txt_2 ptr, txt_3 ptr)

//Infomanager class that resets the items to the appropriate coords

s


*/


StockManager::StockManager(RGBMatrix* canv, Font* txtFont, ImageScroller* stock,string symbol,double currPrice,double startPrice) {
    //TODO: eventually everything should maybe be passed in raw including stock image and txtFont?

    string upArrowPath = "configuration/images/utilities/Green-Up-Arrow-32.ppm";
    //sting downArrowPath = "configuration/images/utilities/Green-Up-Arrow-32.ppm";
    upArrow = new ImageScroller(canv,1,50);
    //downArrow = new ImageScroller(canvas,1,50);
    upArrow->LoadPPM(upArrowPath.c_str());
    //downArrow->LoadPPM(downArrowPath.c_str());

    //Have the current and open priced passed in
    this->currPrice = currPrice;
    this->startPrice = startPrice;
    this->currDiff = currPrice - startPrice;

    //Convert the price to a string with the designated amount of precision
    std::stringstream pStream;
    pStream << std::fixed << std::setprecision(2) << currPrice;
    std::string priceStr = pStream.str();

    //Create the the textual items for the Stock display
    ticker = new Item(64,15,symbol,0,txtFont,this->white_color,64);
    //yes
    if (this->currDiff < 0) {
        price = new Item(64,30,priceStr.c_str(),0,txtFont,this->red_color,64);
        //TODO: Add diff txt for red
    } else {
        //TODO: Add diff txt for green
        price = new Item(64,30,priceStr.c_str(),0,txtFont,this->green_color,64);
    }

    this->stock = stock;
    this->ticker = ticker;
    this->price = price;
    this->arrow = upArrow;

}


void StockManager::resetLocations() {

    int stockWidth = stock->getImageWidth();
    ////cout << "width: " << stockWidth << endl;
    //Set stock starting point

    //TEMP:
    //arrow->setPosX(-64);
    //stockWidth = stock->getImageWidth();

    //stock->setPosX(-stockWidth);
    stock->setPosX(-64);
    //Set Text starting point
    //TODO: update so not static 64 (i.e use rightBoundry)
    //ticker->setPosX(64+stockWidth+30);
    ticker->setPosX(64 + stockWidth + 3);//+stockWidth+10);
    price->setPosX(64 + stockWidth + 3);

    //arrow->setPosX(-64 + -1*(stockWidth + ticker->getPosEnd()));
    //cout << price->getPosEnd() << endl;
    if (price->getPosEnd() > ticker->getPosEnd()) {
        arrow->setPosX(-1*price->getPosEnd());
    } else {
        arrow->setPosX(-1*ticker->getPosEnd());
    }
    

    ////cout << "stock: " << stock->getPostX() << " ticker: " << price->getPosX() << ", end: " << price->getPosEnd() << endl;
    ////cout << price->getLength() << endl;
    ////cout << arrow->getPostX() << endl;

}


bool StockManager::updateLocations(rgb_matrix::Canvas *c, int rightBoundry) {


    
    stock->Run(c);
    arrow->Run(c);

    //cout << arrow->getPostX() - arrow->getImageWidth() << endl;
    ticker->drawItem(c,rightBoundry);
    price->drawItem(c,rightBoundry);

    //arrow->Run(c);

    //INFO: Images are negative based going right
    //      Words are positive based going right
    
    ////cout << price->getPosEnd() << endl;
    if (arrow->getPosEnd() >= 0) {
        resetLocations();
        return true;
    } else {
        return false;
    }


}


int StockManager::getPosEnd() {
    return arrow->getPosEnd();
}

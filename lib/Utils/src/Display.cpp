#include <Display.hpp>

TFT::TFT(uint8_t cs, uint8_t dc, uint8_t led, uint8_t rst) { 
    ledPin = led;
    dcPin = dc;
    csPin = cs;
    rstPin = rst;
    (*tft) = Adafruit_ST7735(&SPI, csPin, dcPin, rstPin); 
    displayBackroundColor = Display_Color_Black;
    displayTextColor = Display_Color_White;
    displayRot = Display_Landscape_0;
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW); 
}

void TFT::initialise(){ 
    displayOFF();
	digitalWrite(ledPin, LOW); 
    tft->initR(INITR_BLACKTAB); 
    tft->setRotation(2);
    tft->setFont();
    tft->fillScreen(displayBackroundColor);
    tft->setTextColor(displayTextColor);
    tft->setTextSize(1); 
    
    tft->enableDisplay(true);
    displayON();
}


void TFT::printTextCentered(const char* string, int8_t y) { 
    int16_t  x1, y1;
    int16_t  x = 16;
    uint16_t w, h;
    displayOFF();
    tft->setCursor(x,y);
    tft->getTextBounds(string, x, y, &x1, &y1, &w, &h);
    tft->setCursor(64-w/2,y);
    tft->print(string);
    displayON();
}
void TFT::displayOFF() { 
	digitalWrite(ledPin, LOW); 
}
void TFT::displayON() { 
	digitalWrite(ledPin, HIGH); 
}
void TFT::setRotation(DisplayRot rot) {
    displayRot = rot;
}
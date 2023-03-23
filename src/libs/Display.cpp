#include "Display.hpp"

TFT::TFT(uint8_t cs, uint8_t dc, uint8_t led, uint8_t rst) { 
    ledPin = led;
    dcPin = dc;
    csPin = cs;
    rstPin = rst;
    displayBackroundColor = Display_Color_Black;
    displayTextColor = Display_Color_White;
    displayRot = Display_Landscape_0;
    Adafruit_ST7735 dp(csPin, dcPin, rstPin); 
    tft = (Adafruit_ST7735*) malloc(sizeof(Adafruit_ST7735));
    memcpy(tft, &dp, sizeof(Adafruit_ST7735));
}
  
void TFT::initialise(SPIClass *spiClass){ 
	pinMode(ledPin, OUTPUT);
    displayOFF();  
    tft->initR(INITR_BLACKTAB);
    tft->setRotation(displayRot);
    tft->setFont();
    tft->fillScreen(displayBackroundColor);
    tft->setTextColor(displayTextColor);
    tft->setTextSize(1); 
    tft->enableDisplay(true);  
    displayON();
}


void TFT::printTextCentered(const char* str, uint8_t y) {  
    int16_t  x1, y1;
    uint16_t x = 16;
    uint16_t w, h;
    displayOFF();
    tft->getTextBounds(str, 16, 16, &x1, &y1, &w, &h);
    tft->setCursor(64-w/2,y);
    tft->print(str); 
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
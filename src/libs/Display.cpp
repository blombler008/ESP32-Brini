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
    u8g2_for_adafruit_gfx.begin(*tft);
}
  
void TFT::initialise(SPIClass *spiClass){ 
	pinMode(ledPin, OUTPUT);
    displayOFF();  
    u8g2_for_adafruit_gfx.setFontMode(1);
    u8g2_for_adafruit_gfx.setBackgroundColor(displayBackroundColor);
    u8g2_for_adafruit_gfx.setForegroundColor(displayTextColor);
    u8g2_for_adafruit_gfx.setFontDirection((displayRot+1)%4);
    u8g2_for_adafruit_gfx.setFont(u8g2_font_8x13B_tr);

    tft->initR(INITR_BLACKTAB);
    tft->setRotation(displayRot); 
    tft->fillScreen(displayBackroundColor); 
    tft->enableDisplay(true);  
    
    displayON();
}

void TFT::drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    tft->drawLine(x0, y0, x1, y1, displayTextColor);
}
  
void TFT::drawHorizontalLine(uint8_t y, uint8_t gap) {
    drawLine(y, gap, 1);
}

void TFT::drawVerticalLine(uint8_t x, uint8_t gap) {
    drawLine(x, gap, 0);
}

void TFT::drawLine(uint8_t s, uint8_t gap, uint8_t mode) {
    mode %= 2;
    switch (mode)
    {
    case 1:
        tft->drawLine(gap, s, displaySize.width-gap, s, displayTextColor);
        break;
    
    case 0:
        tft->drawLine(s, gap, s, displaySize.height-gap, displayTextColor);
        break;
    }
}

void TFT::printTextCentered(const char* str, uint8_t y) {  
    int16_t  x1, y1; 
    uint16_t w, h;
    displayOFF();
    w = u8g2_for_adafruit_gfx.getUTF8Width(str);
    u8g2_for_adafruit_gfx.setCursor(displaySize.width/2-w/2,y);
    u8g2_for_adafruit_gfx.print(str); 
    displayON();
} 
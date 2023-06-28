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
  
void TFT::begin(SPIClass *spiClass){ 
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
        tft->drawLine(gap, s, displaySize.width-gap-1, s, displayTextColor);
        break;
    
    case 0:
        tft->drawLine(s, gap, s, displaySize.height-gap-1, displayTextColor);
        break;
    }
}

DisplaySize TFT::getSize() {
    return displaySize;
}

void TFT::setFont(const uint8_t * font) {
    u8g2_for_adafruit_gfx.setFont(font);
}

void TFT::clear(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    tft->fillRect(x0, y0, x1-x0, y1-y0, Display_Color_Black);
}

void TFT::addButton(const char *label, uint8_t y) {
    uint8_t x = 8;
    const char* x2 = "";
    
    tft->fillRoundRect(x, y, displaySize.width-x*4, x*2, x, Display_Color_Black);  
    tft->drawRoundRect(x, y, displaySize.width-x*4, x*2, x, Display_Color_White);  
    u8g2_for_adafruit_gfx.setForegroundColor(Display_Color_White); 
    u8g2_for_adafruit_gfx.setBackgroundColor(Display_Color_Black); 
    u8g2_for_adafruit_gfx.setFont(u8g2_font_6x12_tr); 
    print(label, x*2, y+12);
}

void TFT::addSelectedButton(const char *label, uint8_t y) { 
    uint8_t x = 8;
    const char* x2 = "";
    
    tft->fillRoundRect(x, y, displaySize.width-x*4, x*2, x, Display_Color_White);  
    u8g2_for_adafruit_gfx.setForegroundColor(Display_Color_Black); 
    u8g2_for_adafruit_gfx.setBackgroundColor(Display_Color_White); 
    u8g2_for_adafruit_gfx.setFont(u8g2_font_6x12_tr); 
    print(label, x*2, y+12); 
}

void TFT::setCursor(uint8_t x, uint8_t y) {
    tft->setCursor(x, y);
}

void TFT::printTextCentered(const char *str, uint8_t y) {
    int16_t  x1, y1; 
    uint16_t w, h;
    w = u8g2_for_adafruit_gfx.getUTF8Width(str);
    u8g2_for_adafruit_gfx.setCursor(displaySize.width/2-w/2,y);
    u8g2_for_adafruit_gfx.print(str); 
}

void TFT::print(const char *string, uint8_t x, uint8_t y) {
    u8g2_for_adafruit_gfx.setCursor(x,y);
    u8g2_for_adafruit_gfx.print(string); 
}

#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>

typedef uint16_t DisplayColor;
typedef uint8_t DisplayRot;

const uint16_t  Display_Color_Black        = 0x0000;
const uint16_t  Display_Color_Blue         = 0x001F;
const uint16_t  Display_Color_Red          = 0xF800;
const uint16_t  Display_Color_Green        = 0x07E0;
const uint16_t  Display_Color_Cyan         = 0x07FF;
const uint16_t  Display_Color_Magenta      = 0xF81F;
const uint16_t  Display_Color_Yellow       = 0xFFE0;
const uint16_t  Display_Color_White        = 0xFFFF;
 
const DisplayRot  Display_Landscape_0 = 0;
const DisplayRot  Display_Portrait_0  = 1;
const DisplayRot  Display_Landscape_1 = 2;
const DisplayRot  Display_Portrait_1  = 3;

 
class TFT {
    public:
        TFT(uint8_t TFT_CS, uint8_t TFT_DC, uint8_t TFT_LED_PIN, uint8_t TFT_RST);
        void initialise() {initialise(&SPI);}; 
        void initialise(SPIClass* spiClass); 
        void setRotation(DisplayRot rot);
        void printTextCentered(const char* string, uint8_t y);
        void displayON();
        void displayOFF();
    private: 
        Adafruit_ST7735* tft = nullptr;
        uint8_t ledPin;
        uint8_t dcPin;
        uint8_t csPin;
        uint8_t rstPin; 
        DisplayRot displayRot;
        DisplayColor displayTextColor;
        DisplayColor displayBackroundColor;
};

#endif
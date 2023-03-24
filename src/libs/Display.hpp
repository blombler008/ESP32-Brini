#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <u8g2_fonts.h>
#include <U8g2_for_Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>

typedef uint16_t DisplayColor;
typedef uint8_t DisplayRot;

typedef struct {
    uint8_t width;
    uint8_t height;
} DisplaySize;


const uint16_t  Display_Color_Black        = 0x0000;
const uint16_t  Display_Color_Blue         = 0x001F;
const uint16_t  Display_Color_Red          = 0xF800;
const uint16_t  Display_Color_Green        = 0x07E0;
const uint16_t  Display_Color_Cyan         = 0x07FF;
const uint16_t  Display_Color_Magenta      = 0xF81F;
const uint16_t  Display_Color_Yellow       = 0xFFE0;
const uint16_t  Display_Color_White        = 0xFFFF;
 
const DisplayRot  Display_Portrait_0  = 0;
const DisplayRot  Display_Landscape_0 = 1;
const DisplayRot  Display_Portrait_1  = 2;
const DisplayRot  Display_Landscape_1 = 3;


 
class TFT {
    public:
        TFT(uint8_t TFT_CS, uint8_t TFT_DC, uint8_t TFT_LED_PIN, uint8_t TFT_RST);
        void initialise() {initialise(&SPI);}; 
        void initialise(SPIClass* spiClass); 
        void setRotation(DisplayRot rot)  { 
            displayRot = rot; 
            switch (rot) 
            { 
            case 1:
            case 3:
                displaySize.width = 160;
                displaySize.height = 128;
                break;
            
            case 0:
            case 2:
                displaySize.width = 128;
                displaySize.height = 160;
                break;
             
            }
        }
        void setColor(DisplayColor color) { displayTextColor = color ; }
        void setBackgroundColor(DisplayColor color) { displayBackroundColor = color ; }

        void setCursor(const char* string); 
        void printTextCentered(const char* string, uint8_t y); 
        void print(const char* string, uint8_t x, uint8_t y); 

        void drawHorizontalLine(uint8_t y, uint8_t gap);
        void drawVerticalLine(uint8_t x, uint8_t gap);
        void drawLine(uint8_t s, uint8_t gap, uint8_t mode);
        void drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

        void displayON()  { digitalWrite(ledPin, HIGH); };
        void displayOFF()  { digitalWrite(ledPin, LOW); };
        
    private: 
        Adafruit_ST7735* tft = nullptr;
        U8G2_FOR_ADAFRUIT_GFX u8g2_for_adafruit_gfx;
        uint8_t ledPin;
        uint8_t dcPin;
        uint8_t csPin;
        uint8_t rstPin; 
        DisplayRot displayRot;
        DisplayColor displayTextColor;
        DisplayColor displayBackroundColor;
        DisplaySize displaySize;

};

#endif
#ifndef MENU_HPP
#define MENU_HPP

#include "Display.hpp"
#include "Encoder.hpp"
#include <stdio.h>
#include <Ticker.h>
typedef struct {  
    int id = -1;
    String itemName;
} MenuItem_t;

// TODO: Callback fuers selected item;

class Menu {
    public: 
        Menu(TFT* display, Encoder* encoder, const char* title);
        void begin();
        void addItem(int id, const char* label);
        void update();
        void redraw();
        void changeOffset();
        void scrollDown();
        void scrollUp();
        void select();
        const char* getTitle();
        int getLineHeight();
        int getSelectedItem();
        const uint8_t * getTitleFont();
        const uint8_t * getButtonFont();
        int getGap();
        void setTitle(const char* title);
        
        TFT* display;
        Encoder* encoder;

    private:
        Ticker tkTitle;
        const uint8_t * fontTitle = u8g2_font_8x13B_tr;
        const uint8_t * fontButton = u8g2_font_6x12_tr;
        int currentSelected = 0;
        int lastItem = 0;
        const char* title;
        
        MenuItem_t* items;
        MenuItem_t* displayItems [5];
        int lineheight = 20, gap = 8;

};

#endif
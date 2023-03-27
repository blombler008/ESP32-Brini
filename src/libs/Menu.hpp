#ifndef MENU_HPP
#define MENU_HPP

#include "Display.hpp"
#include "Encoder.hpp"
#include <stdio.h>
typedef struct MenuItem { 
    int selected = 0;
    int id = -1;
    String itemName;
} MenuItem_t;

class Menu {
    public: 
        Menu(TFT* display, Encoder* encoder, const char* title);
        void initialize();
        void addItem(int id, const char* label);
        void update();
        void scrollDown();
        void scrollUp();
        void select();
        int getSelectedItem();
        
        TFT* display;
        Encoder* encoder;

    private:
        int currentSelected = 0;
        int lastItem = 0;
        const char* title;
        
        MenuItem_t* items;
        int lineheight = 20, gap = 8;

};


#endif
#ifndef DISPLAY_HPP
#define DISPLAY_HPP 
#include <U8g2lib.h>
#include <U8x8lib.h>

class Display {
    public:  
        Display();   
        void begin();
        void printTimeToDisplay(int second, int minute, int hour) ;
};
 

#endif
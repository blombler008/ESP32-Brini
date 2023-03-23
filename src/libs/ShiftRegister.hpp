#ifndef SHIFT_REGISTER_HPP
#define SHIFT_REGISTER_HPP 

#define LSBFIRST 0
#define MSBFIRST 1
#include <FreeRTOS.h> 
#include <Arduino.h>
#include <driver/gpio.h>

class ShiftRegister {
    public:
        ShiftRegister(uint8_t rck, uint8_t clk, uint8_t data); 
        void initialise(); 
        void out(uint8_t data);
        uint8_t in();
        void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t val, uint8_t bitOrder = LSBFIRST);
        uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder = LSBFIRST);
    private:  
        uint8_t RCK_PIN;
        uint8_t CLK_PIN;
        uint8_t DATA_PIN; 

};

#endif
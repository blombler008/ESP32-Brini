#include <ShiftRegister.hpp>  

ShiftRegister::ShiftRegister(uint8_t rck, uint8_t clk, uint8_t data) {
	RCK_PIN = rck;
	CLK_PIN = clk;
	DATA_PIN = data;
	pinMode(RCK_PIN, OUTPUT);
	pinMode(CLK_PIN, OUTPUT); 
	pinMode(DATA_PIN, OUTPUT);
	digitalWrite(RCK_PIN, LOW);
	digitalWrite(CLK_PIN, LOW);
	digitalWrite(DATA_PIN, LOW);  
} 

void ShiftRegister::out(uint8_t data) { 
    shiftOut(DATA_PIN, CLK_PIN, data);   
	digitalWrite(RCK_PIN, HIGH); 
	digitalWrite(RCK_PIN, LOW);
}
 
uint8_t ShiftRegister::in() {
    return shiftIn(DATA_PIN, CLK_PIN);
}
 
uint8_t ShiftRegister::shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
    uint8_t value = 0;
    uint8_t i;

    for(i = 0; i < 8; ++i) {
        //digitalWrite(clockPin, HIGH);
        if(bitOrder == LSBFIRST)
            value |= digitalRead(dataPin) << i;
        else
            value |= digitalRead(dataPin) << (7 - i);
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
    return value;
}

void ShiftRegister::shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t val, uint8_t bitOrder) {
    uint8_t i;

    for(i = 0; i < 8; i++) {
        if(bitOrder == LSBFIRST)
            digitalWrite(dataPin, !!(val & (1 << i)));
        else
            digitalWrite(dataPin, !!(val & (1 << (7 - i))));

        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }
}
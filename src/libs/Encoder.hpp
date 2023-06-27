#ifndef ENCODER_HPP
#define ENCODER_HPP
#include <driver/gpio.h>
#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>
#include <freertos/queue.h> 

typedef enum {
    RIGHT  = 0x200,
    LEFT   = 0x201,
    BUTTON = 0x100
} encoder_actions_t;

typedef void (*callback)(encoder_actions_t, void*);
void IRAM_ATTR isr_sw(void* o);
void IRAM_ATTR isr_rot(void* o);

class Encoder {
    public: 
        Encoder(uint8_t A, uint8_t B, uint8_t SW) { Encoder::A = A; Encoder::B = B; Encoder::SW = SW; };
        void begin();
        void setCallback(callback data, void* type);
        void loop();
        uint8_t getB();
        uint8_t getA();
        uint8_t getSwitch();
        void enqueue(uint8_t cmd, uint8_t data);

    private:
        callback cb;
        void* cls;
        QueueHandle_t queue;
        uint8_t A;
        uint8_t B;
        uint8_t SW; 

};

#endif
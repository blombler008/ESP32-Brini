#include "Encoder.hpp"
 

void Encoder::begin() {
    queue = xQueueCreate(50, sizeof(uint16_t));

    gpio_config_t confA;
    confA.intr_type = GPIO_INTR_NEGEDGE;
    confA.mode = GPIO_MODE_INPUT;
    confA.pin_bit_mask = 1<<A;
    confA.pull_down_en = GPIO_PULLDOWN_DISABLE;
    confA.pull_up_en = GPIO_PULLUP_DISABLE; 

    gpio_config_t confB;
    confB.intr_type = GPIO_INTR_DISABLE;
    confB.mode = GPIO_MODE_INPUT;
    confB.pin_bit_mask = 1<<B;
    confB.pull_down_en = GPIO_PULLDOWN_DISABLE;
    confB.pull_up_en = GPIO_PULLUP_DISABLE; 

    gpio_config_t confsw;
    confsw.intr_type = GPIO_INTR_NEGEDGE;
    confsw.mode = GPIO_MODE_INPUT;
    confsw.pin_bit_mask = 1<<sw;
    confsw.pull_down_en = GPIO_PULLDOWN_DISABLE;
    confsw.pull_up_en = GPIO_PULLUP_DISABLE;    

    gpio_config(&confA);
    gpio_config(&confB);
    gpio_config(&confsw);
    gpio_intr_enable(gpio_num_t(A));
    gpio_intr_enable(gpio_num_t(sw));
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LEVEL2); 
    gpio_isr_handler_add(gpio_num_t(A), isr_rot, this); 
    gpio_isr_handler_add(gpio_num_t(sw), isr_sw, this);
}

void Encoder::setCallback(callback c, void* ty) {
    cb = c;
    cls = ty;
}

void Encoder::loop() {
    uint16_t data = 0;
    xQueueReceiveFromISR(queue, &data, 0); 
    if(data != 0) { 
        (*cb)(encoder_actions(data), cls);  
    }  
}

uint8_t Encoder::getB() {
    return B;
}

uint8_t Encoder::getA() {
    return A;
}
 
uint8_t Encoder::getSW() {
    return sw;
}

void Encoder::enqueue(uint8_t cmd, uint8_t data) {
    uint16_t dataToSend = cmd << 8 | data;
    xQueueSendToBackFromISR(queue, &dataToSend, 0);
}

void IRAM_ATTR isr_sw(void* o) {
    //                    0x0100
    ((Encoder*) o)->enqueue(1, 0);
}

void IRAM_ATTR isr_rot(void* o) {
    
    const int pin = ((Encoder*) o)->getB();
    int level = gpio_get_level(gpio_num_t(pin)); 
    //                    0x0202 oder 0x0201
    ((Encoder*) o)->enqueue(2, level);
}

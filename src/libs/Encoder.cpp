#include "Encoder.hpp"

const int flags = ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED | ESP_INTR_FLAG_LEVEL2;

void setGPIOConfig(gpio_config_t *conf, uint8_t pin) { 
    if(gpio_config(conf) != ESP_OK) 
        printf("Failed to set GPIO config for pin %d\n", pin);
}

void setGPIOisr(gpio_num_t gpio_num, gpio_isr_t isr_handler, void *args) {
    if(gpio_intr_enable(gpio_num) != ESP_OK) 
        printf("Failed to enable GPIO intr for pin %d\n", gpio_num);
 
    if(gpio_isr_handler_add(gpio_num, isr_handler, args) != ESP_OK) 
        printf("Failed to set GPIO ISR Handler for pin %d\n", gpio_num);
}

gpio_config_t createConf(uint8_t pin, gpio_int_type_t intr) {
    gpio_config_t result;

    result.intr_type = intr;
    result.mode = GPIO_MODE_INPUT;
    result.pin_bit_mask = 1<<pin;
    result.pull_down_en = GPIO_PULLDOWN_ENABLE;
    result.pull_up_en = GPIO_PULLUP_DISABLE; 

    return result;
}

void Encoder::begin() {
    queue = xQueueCreate(50, sizeof(uint16_t)); 
    gpio_config_t confA = createConf(A, GPIO_INTR_NEGEDGE);
    gpio_config_t confB= createConf(B, GPIO_INTR_DISABLE);
    gpio_config_t confSW = createConf(SW, GPIO_INTR_NEGEDGE);
 
    setGPIOConfig(&confA, A);
    setGPIOConfig(&confB, B);
    setGPIOConfig(&confSW, SW);
     
    if(gpio_install_isr_service(flags) != ESP_OK) 
        printf("Failed to install gpio isr service\n");

    setGPIOisr(gpio_num_t(SW), isr_sw, this); 
    setGPIOisr(gpio_num_t(A), isr_rot, this); 
}

void Encoder::setCallback(callback c, void* ty) {
    cb = c;
    cls = ty;
}

void Encoder::loop() {
    uint16_t data = 0;
    xQueueReceiveFromISR(queue, &data, 0); 
    if(data != 0) { 
        (*cb)(encoder_actions_t(data), cls);  
    }  
}

uint8_t Encoder::getB() {
    return B;
}

uint8_t Encoder::getA() {
    return A;
}
 
uint8_t Encoder::getSwitch() {
    return SW;
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

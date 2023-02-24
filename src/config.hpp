#include <Arduino.h>  
#include <pins_arduino.h>
#include "PrintHelper.hpp" 
// #include "display.hpp"
 
#include <FreeRTOS.h>
#include <task.h>
#include <Wire.h>   
#include <SPI.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h> 

#include <WiFi.h> 
#include <LuaWrapper.h> 

#ifndef CONFIG_HPP
#define CONFIG_HPP


// Constants 
#define WIFI_SSID "Wickinger"
#define WIFI_HOSTNAME "ESP32S2.local"
#define WIFI_PASSWORD "EJYBmZ3pRdkHu614B8DC" // secret

// serial
#define SERIAL_BAUD 115200

// display
#define DISPLAY_ADRESS 0x3C
#define OLED_ROTATION U8G2_R2

// Register Serial Pins
#define SR_CLK_PIN      4   // shift clock
#define SR_DATA_PIN     5   // data
#define SR_OE_PIN       6   // output enable
#define SR_RCK_PIN      7   // storage clock
#define SR_CLR_PIN      15  // storage & shift register clear


// RFID SPI pins
#define RFID_SCK        36  // CLOCK
#define RFID_MISO       38  // DATA IN
#define RFID_MOSI       37  // DATA OUT
#define RFID_CS         35  // SELECT
#define RFID_RST        39  // RESET

//  
#define DEBUG true



#endif
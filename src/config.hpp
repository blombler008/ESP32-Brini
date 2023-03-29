#include <Arduino.h>  
#include <pins_arduino.h>
#include <libs/PrintHelper.hpp>
#include <libs/fswrapper.hpp>
#include <libs/Display.hpp>
#include <libs/ShiftRegister.hpp>
#include <libs/LuaHandler.hpp>
#include <libs/Encoder.hpp>
#include <libs/Menu.hpp>
#include <libs/WiFiHelper.hpp>

#include <phy_init_data.h>
#include <FreeRTOS.h>
#include <task.h>

#include <Wire.h>   
#include <SPI.h>

#include <Adafruit_NeoPixel.h> 

#include <WiFi.h> 
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WebServer.h>

#include <LuaWrapper.h> 
#include <LittleFS.h>
 
#include <MFRC522.h> 
#include <time.h>
 
#define FORMAT_LITTLEFS_IF_FAILED false

#ifndef CONFIG_HPP
#define CONFIG_HPP


// Constants 
#define WIFI_SSID       "Wickinger"
#define WIFI_HOSTNAME   "ESP32S2.local"
#define WIFI_PASSWORD   "EJYBmZ3pRdkHu614B8DC" // secret "DON'T TELL ANYONE"

// serial
#define SERIAL_BAUD     115200
  
// Register Serial Pins
#define SR_CLK_PIN      4   // shift clock
#define SR_DATA_PIN     5   // data
#define SR_OE_PIN       6   // output enable
#define SR_RCK_PIN      7   // storage clock
#define SR_CLR_PIN      15  // storage & shift register clear

// ENCODER SWITCHES
#define ENC_A           1
#define ENC_B           2 
#define ENC_SW          3

// SPI pins
#define SPI_SCK         36  // CLOCK
#define SPI_MISO        34  // DATA IN
#define SPI_MOSI        35  // DATA OUT

#define RFID_CS         37  // Chip SELECT
#define RFID_RST        33  // RESET

#define TFT_CS          21
#define TFT_DC          20  
#define TFT_RST         19
#define TFT_LED_PIN     38

// Menu Information

#define GAP             8
#define LINEHIGHT       20
#define TEXTHEIGHT      4
#define TITLEHEIGHT     16
#define TITLE           "Cocktail-Mixer"



#endif
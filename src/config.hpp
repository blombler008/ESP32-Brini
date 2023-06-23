
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
#include <libs/MFRC.hpp>


#include <phy_init_data.h>
#include <FreeRTOS.h>
#include <task.h>

#include <Wire.h>   
#include <SPI.h> 

#include <Adafruit_NeoPixel.h> 
 
#include <time.h>
 

#ifndef CONFIG_HPP
#define CONFIG_HPP


// WIFI setup
#define WIFI_SSID       "Wickinger"
#define WIFI_HOSTNAME   "ESP32S2.local"
#define WIFI_PASSWORD   "EJYBmZ3pRdkHu614B8DC" // secret "DON'T TELL ANYONE"
#define WIFI_SERVER_PORT 8080
#define WIFI_UDP_PORT 8888

// serial Setup
#define SERIAL_BAUD     115200
  
// Littlefs Setup
#define FORMAT_LITTLEFS_IF_FAILED false

// Register Serial Pins
#define SR_CLK_PIN      6   // shift clock
#define SR_DATA_PIN     4   // data
#define SR_OE_PIN       5   // output enable
#define SR_RCK_PIN      15   // storage clock
#define SR_CLR_PIN      7  // storage & shift register clear

// ENCODER SWITCHES
#define ENC_A           1
#define ENC_B           2 
#define ENC_SW          3

// SSR Relai
#define SSR_D1          10
#define SSR_D2          11
#define SSR_D3          12
#define SSR_D4          13


// SPI pins
#define SPI_SCK         36  // CLOCK
#define SPI_MISO        38  // DATA IN
#define SPI_MOSI        37  // DATA OUT

#define RFID_CS         35  // Chip SELECT
#define RFID_RST        39  // RESET

#define TFT_CS          19
#define TFT_DC          21  
#define TFT_RST         20
#define TFT_LED_PIN     34

// Menu Information

#define GAP             8
#define LINEHIGHT       20
#define TEXTHEIGHT      4
#define TITLEHEIGHT     16
#define TITLE           "Cocktail-Mixer"

#define arrlen(x) sizeof(x)/4

#endif
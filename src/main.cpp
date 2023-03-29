#include <config.hpp>   

WiFiHelper wifi;
Encoder encoder(ENC_A, ENC_B, ENC_SW); 
TFT display(TFT_CS, TFT_DC, TFT_LED_PIN, TFT_RST);
ShiftRegister SR(SR_RCK_PIN, SR_CLK_PIN, SR_DATA_PIN); 
MFRC mfrc;
LuaHandler luaHandler(FORMAT_LITTLEFS_IF_FAILED); 
Menu menu(&display, &encoder, TITLE);

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 18, NEO_GRB+NEO_KHZ800);

    
void setup() { 
    Serial.begin(115200);  
    esp_phy_erase_cal_data_in_nvs();
    pixel.begin();  
    
    luaHandler.initialize();
    luaHandler.start();

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    SR.initialise(); 
    SR.out(0xaa); 
    vTaskDelay(200); 
    SR.out(0x55); 
    vTaskDelay(200);
	SR.out(0);
  
    display.setRotation(Display_Landscape_1);
    encoder.initialize(); 
    display.initialise(&SPI);  
    menu.initialize();

    menu.addItem(0, "Apfel Saft");
    menu.addItem(1, "Kaffee");
    menu.addItem(2, "BaKi");
    menu.addItem(3, "Orangen Saft");
    menu.addItem(4, "Multivitamin Saft");
    menu.addItem(5, "Vodka");
    menu.addItem(6, "Tee");
    menu.addItem(7, "Tee mit Schuss");

    menu.update();
    
    mfrc.begin(RFID_CS, RFID_RST); 
    vTaskDelay(4);
    mfrc.PCD_DumpVersionToSerial(); 
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks...")); 

    WiFiHelperConfig_t wifiConfig;
    wifiConfig.hostname = WIFI_HOSTNAME;
    wifiConfig.password = WIFI_PASSWORD;
    wifiConfig.server_port = 8080;
    wifiConfig.udp_port = 8888;
    wifiConfig.ssid = WIFI_SSID; 

    wifi.begin(&wifiConfig);
}

uint8_t data = 0;

void loop() {
    SR.out(wifi.getWifiStatus());
    encoder.loop(); 
}

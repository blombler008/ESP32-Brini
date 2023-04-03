#include <config.hpp>   

WiFiHelper wifi;
MFRC mfrc(RFID_CS, RFID_RST);
Encoder encoder(ENC_A, ENC_B, ENC_SW); 
TFT display(TFT_CS, TFT_DC, TFT_LED_PIN, TFT_RST);
ShiftRegister SR(SR_RCK_PIN, SR_CLK_PIN, SR_DATA_PIN); 
// LuaHandler luaHandler(FORMAT_LITTLEFS_IF_FAILED); 
Menu menu(&display, &encoder, TITLE);
SPIClass vspi;
// Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 18, NEO_GRB+NEO_KHZ800);

const char* strings[] = {
    "Apfel Saft",
    "Orangen Saft",
    "BaKi",
    "Apfel Kirsch",
    "Vodka",
    "Vodka E+",
    "BaKi mit Schuss",
    "Apfel mit schuss"
};

void setup() { 
    esp_phy_erase_cal_data_in_nvs();
    
    Serial.begin(SERIAL_BAUD);
    // pixel.begin();  
    
    // luaHandler.initialize();
    // luaHandler.start();

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI); 
    SPI.setFrequency(10000000u);

    SR.initialise(); 
    SR.out(0xaa); 
    vTaskDelay(200); 
    SR.out(0x55); 
    vTaskDelay(200);
	SR.out(0);
  
    encoder.begin();

    display.setRotation(Display_Landscape_1);
    display.begin(&SPI);
    
    menu.begin(); 
    for (int i = 0; i < sizeof(strings)/sizeof(char*); i++) {
        menu.addItem(i, strings[i]);
    }  
    menu.update();
    
    mfrc.begin(); 
    vTaskDelay(4);
    mfrc.PCD_DumpVersionToSerial(); 


    WiFiHelperConfig_t wifiConfig;
    wifiConfig.hostname = WIFI_HOSTNAME;
    wifiConfig.password = WIFI_PASSWORD;
    wifiConfig.server_port = WIFI_SERVER_PORT;
    wifiConfig.udp_port = WIFI_UDP_PORT ;
    wifiConfig.ssid = WIFI_SSID; 

    wifi.begin(&wifiConfig);
}

uint8_t data = 0;

void loop() {
    SR.out(wifi.getWifiStatus());
    encoder.loop();
    if(!mfrc.hasReadCard()) { return; }
    
    MFRC522::Uid uid = mfrc.getUID();
    String uidBuff = mfrc.PCD_UIDToString(uid); 
    menu.setTitle(uidBuff.c_str());
        
    if(wifi.getWifiStatus() != TCPCONNECTED) { return; } 

    WiFiCommand cmd = WiFiCommand();  
    char buff[30]; 
    sprintf(buff, "uid set %s\n", uidBuff);
    
    memcpy(cmd.data, buff, 30);
    wifi.sendData(&cmd);
}

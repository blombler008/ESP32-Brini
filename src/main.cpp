#include <config.hpp>   

WiFiHelper wifi;
MFRC mfrc(RFID_CS, RFID_RST);
// Encoder encoder(ENC_A, ENC_B, ENC_SW); 
// TFT display(TFT_CS, TFT_DC, TFT_LED_PIN, TFT_RST);
ShiftRegister SR(SR_RCK_PIN, SR_CLK_PIN, SR_DATA_PIN); 
// LuaHandler luaHandler(FORMAT_LITTLEFS_IF_FAILED); 
// Menu menu(&display, &encoder, TITLE);

// Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 18, NEO_GRB+NEO_KHZ800);

void setup() { 
    esp_phy_erase_cal_data_in_nvs();
    
    Serial.begin(SERIAL_BAUD);
    // pixel.begin();  
    
    // luaHandler.initialize();
    // luaHandler.start();

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    SR.initialise(); 
    SR.out(0xaa); 
    vTaskDelay(200); 
    SR.out(0x55); 
    vTaskDelay(200);
	SR.out(0);
  
    // display.setRotation(Display_Landscape_1);
    // encoder.initialize(); 
    // display.initialise(&SPI);  
    // menu.initialize();

    // menu.addItem(0, "Apfel Saft");
    // menu.addItem(1, "Kaffee");
    // menu.addItem(2, "BaKi");
    // menu.addItem(3, "Orangen Saft");
    // menu.addItem(4, "Multivitamin Saft");
    // menu.addItem(5, "Vodka");
    // menu.addItem(6, "Tee");
    // menu.addItem(7, "Tee mit Schuss");

    // menu.update();
    
    mfrc.begin(); 
    vTaskDelay(4);
    mfrc.PCD_DumpVersionToSerial(); 


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
    // encoder.loop();  
    if(mfrc.hasReadCard()) { 
        MFRC522::Uid uid = mfrc.getUID();
        char buff[30]; 
        String uidBuff = mfrc.PCD_UIDToString(uid);
        sprintf(buff, "uid set %s\n", uidBuff);
        CMDMessage msg = CMDMessage();  
        memcpy(msg.data, buff, 30);
            
        if(wifi.getWifiStatus() == TCPCONNECTED) { 
                
            wifi.sendData(&msg);
        }

    }


    
    // if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        
    //     printUID(mfrc522.uid);
    //     client.printf("uid set ");
    //     for (int i = 0; i < mfrc522.uid.size; i++)
    //     {
    //         if (i > 0) client.printf(":");
    //         client.printf("%02X", mfrc522.uid.uidByte[i]);
    //     }
    //     client.printf("\n");
    //     client.flush();  
        
    // }
}

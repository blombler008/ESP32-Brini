#include <config.hpp>   
#include "Cocktail.hpp"

WiFiHelper wifi;
MFRC mfrc(RFID_CS, RFID_RST);
Encoder encoder(ENC_A, ENC_B, ENC_SW); 
TFT display(TFT_CS, TFT_DC, TFT_LED_PIN, TFT_RST);
ShiftRegister SR(SR_RCK_PIN, SR_CLK_PIN, SR_DATA_PIN); 
// LuaHandler luaHandler(FORMAT_LITTLEFS_IF_FAILED); 
Menu menu(&display, &encoder, TITLE);
SPIClass vspi;
// Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 18, NEO_GRB+NEO_KHZ800);


bool requireUid = false;
int lastButtonId = -1;
const char* lastScannedUid;

Cocktail *cocktails[8];

void SetupCocktail() {
    CocktailPump currentPump1;
    currentPump1.pump = 1;
    CocktailPump currentPump2;
    currentPump2.pump = 2;
    CocktailPump currentPump3;
    currentPump3.pump = 3;
    CocktailPump currentPump4;
    currentPump4.pump = 4;
    CocktailPump pumps[] = {currentPump1, currentPump2, currentPump3, currentPump4};

    currentPump1.time = 0;
    currentPump2.time = 2500;
    currentPump3.time = 0;
    currentPump4.time = 0;
    (*cocktails[0]) = Cocktail("Apfel Saft", pumps); 
    currentPump1.time = 0;
    currentPump2.time = 0;
    currentPump3.time = 1250;
    currentPump4.time = 1250;
    (*cocktails[1]) = Cocktail("BaKi", pumps);
    currentPump1.time = 0;
    currentPump2.time = 1250;
    currentPump3.time = 1250;
    currentPump4.time = 0;
    (*cocktails[2]) = Cocktail("Apfel Kirsch", pumps);
    currentPump1.time = 2500;
    currentPump2.time = 0;
    currentPump3.time = 0;
    currentPump4.time = 0;
    (*cocktails[3]) = Cocktail("Vodka", pumps); 
    currentPump1.time = 250;
    currentPump2.time = 0;
    currentPump3.time = 1125;
    currentPump4.time = 1125;
    (*cocktails[4]) = Cocktail("BaKi mit Schuss", pumps);
    currentPump1.time = 250;
    currentPump2.time = 2250;
    currentPump3.time = 0;
    currentPump4.time = 0;
    (*cocktails[5]) = Cocktail("Apfel mit schuss", pumps);
 
}
 
int ids[20];

void CocktailUsed(int cocktail, const char* uid) {
    if(String(uid).isEmpty() && requireUid) return;

    Serial.printf("Cocktail %s wurde benutzt\n", cocktails[cocktail]->getName());

    menu.setTitle("");
    lastButtonId = -1;
    lastScannedUid = "";
    


}

void menuItemSelected(MenuItem_t* item) {
    if(wifi.getWifiStatus()==TCPCONNECTED && requireUid) { 
        wifi.sendData("uid clear");
    }
    CocktailUsed(item->id, lastScannedUid);
}

void setupWiFI(WiFiClient *client) { 
    client->println("but clear"); 
    String requireUidString = String("uid req ");
    requireUidString.concat((requireUid ? "true" : "false"));
    client->println(requireUidString.c_str());
    char buff[30];
    for (int i = 0; i < arrlen(cocktails) ; i++) {
        sprintf(buff, "add %i %s", i, cocktails[i]->getName());
        client->println(buff);
    }   
    client->println("uid clear"); 
}

void onCommandRecievd(const char* cmd) {
    String command = String(cmd);
    if(command.startsWith("but")) { 
        command = command.substring(4);
        if(command.startsWith("added")) { 
            String viewIdString = command.substring(6, command.indexOf(" ", 6));
            String buttonIdString = command.substring(7+viewIdString.length());
             
            int view = viewIdString.toInt();
            int button = buttonIdString.toInt();
            ids[button] = view;
            return;    
        }
        if(command.startsWith("del")) { return; }
        int viewId = command.toInt();
        int button = 0;
        for (int i = 0; i < arrlen(cocktails); i++) {
            if(ids[i] == viewId) {
                button = i;
                break;
            }
        }
        if(requireUid) {
            lastButtonId = button;
            wifi.sendData("uid get");
            return;
        }
        CocktailUsed(button, "");
    }
    if(command.startsWith("uid ")) {
        command = command.substring(4);
        CocktailUsed(lastButtonId, command.c_str());
    }
}

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
    menu.setCallback(menuItemSelected);
    for (int i = 0; i < arrlen(cocktails) ; i++) {
        menu.addItem(i, cocktails[i]->getName().c_str());
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
    wifi.setWiFISetupFunction(setupWiFI);
    wifi.setRecieveCommandFunction(onCommandRecievd);

    SetupCocktail();
}
 
void loop() {
    SR.out(wifi.getWifiStatus());
    encoder.loop();

    if(mfrc.hasReadCard() && requireUid) {
        MFRC522::Uid uid = mfrc.getUID();
        String uidBuff = mfrc.PCD_UIDToString(uid); 
        menu.setTitle(uidBuff.c_str());
        lastScannedUid = uidBuff.c_str();
        if(wifi.getWifiStatus() != TCPCONNECTED) { return; } 
   
        wifi.sendData(("uid set " + uidBuff).c_str());
    }
    

}

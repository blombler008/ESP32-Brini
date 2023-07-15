#include <config.hpp>   
#include "Cocktail.hpp"

WiFiHelper wifi;
MFRC mfrc(RFID_CS, RFID_RST);
Encoder encoder(ENC_A, ENC_B, ENC_SW); 
TFT display(TFT_CS, TFT_DC, TFT_LED_PIN, TFT_RST);
ShiftRegister SR(SR_RCK_PIN, SR_CLK_PIN, SR_DATA_PIN); 
Menu menu(&display, &encoder, TITLE);
SPIClass vspi; 

// LuaHandler luaHandler(FORMAT_LITTLEFS_IF_FAILED);  
// Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 18, NEO_GRB+NEO_KHZ800);
 
bool rfidKarteErforderlich = true;
int zuletztAusgeWaehlterCocktail = -1;
char letzteGescanteKarte[12];

#define cocktails_size 8 //Wie viele maximale Cocktails gibt es (kann man anpassen)

// Array = ein liste von Speicherfeldern
// In diesen Array bzw. in dieser Liste, werden die Cocktails gespeichert
Cocktail *cocktails[cocktails_size]; 

// Hier werden die Pumpen und die Cocktails initailisiert, und festgelegt
// wie lange die pumpen die fluessigkeit pumpen sollen
// 0 = aus, in Millisekunden
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

    // eine Speicherzelle wrid mit "[n]" angesprochen. (n von 0 bis cocktails_size-1) 
    // in die speicherzellen von den Cocktails speichern wir Neue Cocktails
    currentPump1.time = 0;
    currentPump2.time = 2500;
    currentPump3.time = 0;
    currentPump4.time = 0;
    cocktails[0] = new Cocktail("Apfel Saft", pumps); 
    currentPump1.time = 0;
    currentPump2.time = 0;
    currentPump3.time = 1250;
    currentPump4.time = 1250;
    cocktails[1] = new Cocktail("BaKi", pumps);
    currentPump1.time = 0;
    currentPump2.time = 1250;
    currentPump3.time = 1250;
    currentPump4.time = 0;
    cocktails[2] = new Cocktail("Apfel Kirsch", pumps);
    currentPump1.time = 2500;
    currentPump2.time = 0;
    currentPump3.time = 0;
    currentPump4.time = 0;
    cocktails[3] = new Cocktail("Vodka", pumps); 
    currentPump1.time = 250;
    currentPump2.time = 0;
    currentPump3.time = 1125;
    currentPump4.time = 1125;
    cocktails[4] = new Cocktail("BaKi mit Schuss", pumps);
    currentPump1.time = 250;
    currentPump2.time = 2250;
    currentPump3.time = 0;
    currentPump4.time = 0;
    cocktails[5] = new Cocktail("Apfel mit schuss", pumps);
 
}

 // Jeder Cocktail bekommt ihre eigene ID (n)
 // In dieser liste werden so viele IDs gespeichert, wie es Cocktails geben darf
int ids[cocktails_size];

Cocktail getCocktailFromID(int id) {
    return *(cocktails[id]);
}

void CocktailUsed(int cocktailID, const char* uid) {
    String uidStr = String(uid);
    if(uidStr.isEmpty() && rfidKarteErforderlich) return;
    Cocktail cocktailUsed = getCocktailFromID(cocktailID);
    
    if(uidStr.isEmpty())
        logprint("Cocktail %s wurde benutzt", cocktailUsed.getName());
    else 
        logprint("%s hat Cocktail %s wurde benutzt", uid, cocktailUsed.getName());

    menu.resetTitle();
    zuletztAusgeWaehlterCocktail = -1;
    
    for (size_t i = 0; i < 12; i++) {
        letzteGescanteKarte[i] = 0;             
    }
    
}

void menuItemSelected(MenuItem_t* item) {
    if(wifi.getWifiStatus()==TCPCONNECTED && rfidKarteErforderlich) { 
        wifi.sendData("uid clear");
    }
    CocktailUsed(item->id, letzteGescanteKarte);    
}

void setupWiFI(WiFiClient *tablet) { 
    tablet->println("but clear"); 
    String requireUidString = String("uid req ");
    requireUidString.concat((rfidKarteErforderlich ? "true" : "false"));
    tablet->println(requireUidString.c_str());
    char buff[30];
    for (int i = 0; i < arrlen(cocktails) ; i++) {
        if(cocktails[i] == 0) continue;
        sprintf(buff, "add %i %s", i, cocktails[i]->getName());
        tablet->println(buff);
    }   
    tablet->println("uid clear"); 
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
        // but 5
        int viewId = command.toInt();
        int button = 0;
        for (int i = 0; i < arrlen(cocktails); i++) {
            if(cocktails[i] == 0) continue;
            if(ids[i] == viewId) {
                button = i;
                break;
            }
        }
        if(rfidKarteErforderlich) {
            zuletztAusgeWaehlterCocktail = button;
            wifi.sendData("uid get");
            return;
        }
        CocktailUsed(button, "");
    }
    if(command.startsWith("uid ")) {
        command = command.substring(4);
        CocktailUsed(zuletztAusgeWaehlterCocktail, command.c_str());
    }
}

void serialInputLoop(void* pv) {
     
    String command = Serial.readString();
    if(!command.isEmpty()) {
        if(command.startsWith("on")) {
            logprint("Turning on %d", command.substring(3).toInt());
        }
        if(command.startsWith("off")) {
            logprint("Turning off %d", command.substring(4).toInt());
            
        }
    } 
}

void setup() { 

    Serial.begin(SERIAL_BAUD);
    // pixel.begin();  
    
    // luaHandler.initialize();
    // luaHandler.start();

    SetupCocktail();
    
    WiFiHelperConfig_t wifiConfig;
    wifiConfig.hostname = WIFI_HOSTNAME;
    wifiConfig.password = WIFI_PASSWORD;
    wifiConfig.server_port = WIFI_SERVER_PORT;
    wifiConfig.udp_port = WIFI_UDP_PORT ;
    wifiConfig.ssid = WIFI_SSID; 

    wifi.begin(&wifiConfig);
    wifi.setWiFISetupFunction(setupWiFI);
    wifi.setRecieveCommandFunction(onCommandRecievd);


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
    // Menu wird auf das Display uebertragen
    menu.begin(); 
    // wenn du im Menu auf dem Display ein Cocktail auswaehlst wird das unterprogramm in der klammer ausgefuehrt
    menu.setCallback(menuItemSelected); 
    for (int i = 0; i < arrlen(cocktails) ; i++) {
        
        if(cocktails[i] == 0) continue; 
        menu.addItem(i, cocktails[i]->getName());
    }  
    menu.update();
    
    mfrc.begin(); 
    vTaskDelay(4);
    mfrc.PCD_DumpVersionToSerial(); 

    xTimerCreate("serialInputLoop", 100 / portTICK_PERIOD_MS, true, NULL, serialInputLoop);

	logprint("Setup Finished");
}


 
void loop() {
    SR.out(wifi.getWifiStatus());
    encoder.loop();
    if(mfrc.hasReadCard() && rfidKarteErforderlich) {
        MFRC522::Uid uid = mfrc.getUID();
        String uidBuff = mfrc.PCD_UIDToString(uid); 
        menu.setTitle(uidBuff.c_str());
        strcpy(letzteGescanteKarte, uidBuff.c_str()); 
        if(wifi.getWifiStatus() != TCPCONNECTED) { return; } 
   
        wifi.sendData(("uid set " + uidBuff).c_str());
    }
    

}


// #include <config.hpp>
// // include the SD library:
 
// LuaHandler luaHandler(FORMAT_LITTLEFS_IF_FAILED); 

// // WeMos D1 esp8266: D8 as standard
// const int spi_sck = 9;
// const int spi_miso = 10;
// const int spi_mosi = 11;
// const int chipSelect = 12;
 
// void printDirectory(File dir, int numTabs);
 
// void setup() {
//   // Open serial communications and wait for port to open:
//   Serial.begin(115200);
//   SPI.begin(spi_sck, spi_miso, spi_mosi); 
 
//   luaHandler.initialize(chipSelect); 
//   Serial.print("\nInitializing SD card...");
  
//   // print the type of card
//   Serial.println();
//   Serial.print("Card type:         ");
//   switch (SD.cardType()) {
//     case CARD_NONE:
//       Serial.println("NONE");
//       break;
//     case CARD_MMC:
//       Serial.println("MMC");
//       break;
//     case CARD_SD:
//       Serial.println("SD");
//       break;
//     case CARD_SDHC:
//       Serial.println("SDHC");
//       break;
//     default:
//       Serial.println("Unknown");
//   }
 
//   // print the type and size of the first FAT-type volume
// //  uint32_t volumesize;
// //  Serial.print("Volume type is:    FAT");
// //  Serial.println(SDFS.usefatType(), DEC);
 
//   Serial.print("Card size:  ");
//   Serial.println((float)SD.cardSize()/1000);
 
//   Serial.print("Total bytes: ");
//   Serial.println(SD.totalBytes());
 
//   Serial.print("Used bytes: ");
//   Serial.println(SD.usedBytes());
   
//   luaHandler.start();

// }
 
// void loop(void) {
// }
  
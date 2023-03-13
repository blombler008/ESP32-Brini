#include <config.hpp>  
#include <phy_init_data.h> 
#include <LuaHandler.hpp>
#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
WiFiUDP udp;
WiFiServer server(8080);
 
MFRC522 mfrc522(RFID_CS, RFID_RST);
Adafruit_ST7735 tft(&SPI, TFT_CS, TFT_DC, TFT_RST); 

void printUID(MFRC522::Uid uid);

void shiftData(uint8_t data) {  
    shiftOut(SR_DATA_PIN, SR_CLK_PIN, LSBFIRST, data);   
	digitalWrite(SR_RCK_PIN, HIGH); 
	digitalWrite(SR_RCK_PIN, LOW);
}


TaskHandle_t fs_lua;
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 18, NEO_GRB+NEO_KHZ800);
void fsinit(void* pvArgs) {
    
    FSWrapper fswrapper;
    fswrapper.begin(FORMAT_LITTLEFS_IF_FAILED);
    fswrapper.listDir("/", 0); 
    
    LuaWrapper lua; 
    LuaHandler luaHandler(lua.L());  

    const String constants = (const String) lua.addConstants();
    lua.Lua_dostring(&constants);
    
    String result = lua.Lua_doFile("/littlefs/test.lua");
    if (!result.isEmpty()) Serial.println(result);
 
    while(true) {
        luaHandler.executeLoop(); 
        uint32_t color = luaHandler.executeSetPixel(); 

        pixel.setPixelColor(0, color);
        pixel.setBrightness(10);
        pixel.show(); 

        luaHandler.wait(50000);
        portYIELD();
    }
    vTaskDelete(fs_lua);
}

const uint16_t  Display_Color_Black        = 0x0000;
const uint16_t  Display_Color_Blue         = 0x001F;
const uint16_t  Display_Color_Red          = 0xF800;
const uint16_t  Display_Color_Green        = 0x07E0;
const uint16_t  Display_Color_Cyan         = 0x07FF;
const uint16_t  Display_Color_Magenta      = 0xF81F;
const uint16_t  Display_Color_Yellow       = 0xFFE0;
const uint16_t  Display_Color_White        = 0xFFFF;

// The colors we actually want to use
uint16_t        Display_Text_Color         = Display_Color_White;
uint16_t        Display_Backround_Color    = Display_Color_Black;

void printTextCentered(Adafruit_ST7735* tft, const char* string, int8_t y) { 
    int16_t  x1, y1;
    int16_t  x=16;
    uint16_t w, h;
    tft->setCursor(x,y);
    tft->getTextBounds(string, x, y, &x1, &y1, &w, &h);
    tft->setCursor(64-w/2,y);
    tft->print(string);
}

void setup() {
    
    Serial.begin(115200);  
    esp_phy_erase_cal_data_in_nvs();
    pixel.begin();

    xTaskCreatePinnedToCore(fsinit, "fs_lua", 10000, NULL, 1, &fs_lua, 1);
     
	pinMode(SR_RCK_PIN, OUTPUT);
	pinMode(SR_CLK_PIN, OUTPUT); 
	pinMode(SR_DATA_PIN, OUTPUT);
	pinMode(TFT_LED_PIN, OUTPUT);
	digitalWrite(SR_RCK_PIN, LOW);
	digitalWrite(SR_CLK_PIN, LOW);
	digitalWrite(SR_DATA_PIN, LOW); 
	digitalWrite(TFT_LED_PIN, LOW); 
    
	shiftData(0xaa);	
    vTaskDelay(200);
	shiftData(0x55);	

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    tft.initR(INITR_BLACKTAB); 
    tft.setRotation(2);
    tft.setFont();
    tft.fillScreen(Display_Backround_Color);
    tft.setTextColor(Display_Text_Color);
    tft.setTextSize(1);
    
    tft.enableDisplay(true);
	digitalWrite(TFT_LED_PIN, HIGH); 
    printTextCentered(&tft, "Cocktail-Mixer", 16);
 
	mfrc522.PCD_Init();
	delay(4);
	mfrc522.PCD_DumpVersionToSerial();
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

    vTaskDelay(200);
	shiftData(0);


    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    server.begin();
	Serial.println("Starting...");
    udp.setTimeout(20);
    server.setNoDelay(true);
    server.setTimeout(20);
    udp.begin(8888);
}

uint8_t data= 0;

void loop() {
    int packetSize = udp.parsePacket();  
    
    shiftData(0x01);
    if (packetSize) {
         
        shiftData(0x03);	
        byte buffer[packetSize];
        udp.read(buffer, packetSize);
        String message = String((char*)buffer);
        Serial.println("[UDP] Received UDP message: " + message);

        udp.beginPacket(udp.remoteIP(), udp.remotePort()); 
        udp.write(WiFi.localIP()[0]); 
        udp.write(WiFi.localIP()[1]); 
        udp.write(WiFi.localIP()[2]); 
        udp.write(WiFi.localIP()[3]); 
        udp.endPacket(); 
        Serial.print("[UDP] Sent IP: ");
        Serial.println(WiFi.localIP().toString()); 
    }
	
	if(server.hasClient()) {
        shiftData(0x07);	
		Serial.println("[TCP] Connecting");
		WiFiClient client = server.available(); 
		while(client.connected()) {   
            if(client.available()) { 
                String data = client.readString();
                String newData = data.substring(0, data.indexOf("\n"));
                Serial.println("[TCP] Received from Tablet: " + newData); 
                     
                client.printf("Hello Tablet!\n"); 
                client.flush();   
                client.printf("add TestButton 1\n"); 
                client.flush(); 
                client.printf("uid clear\n"); 
                client.flush(); 
                break;
            }
        }
		while(client.connected()) {   
			if (client.available()) { 
				String data = client.readString();
				String newData = data.substring(0, data.indexOf("\n"));
				Serial.println("[TCP] Received: " + newData); 
                if(newData.startsWith("but")) { 
                    newData = newData.substring(4, newData.length());
                    if(newData.startsWith("added")) { continue; }
				    Serial.println("[TCP] Button: " + newData); 
                    client.printf("uid get\n");
                    client.flush();  
                }
			}  
            if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
                
                printUID(mfrc522.uid);
                client.printf("uid set ");
                for (int i = 0; i < mfrc522.uid.size; i++)
                {
                    if (i > 0) client.printf(":");
                    client.printf("%02X", mfrc522.uid.uidByte[i]);
                }
                client.printf("\n");
                client.flush();  
                
            }
		} 
		server.stopAll(); 
		Serial.println("[TCP] Connection Closed"); 
        shiftData(0);	
	}  
    

}
 
 
void printUID(MFRC522::Uid uid) { 
	printf("UID OF Device: ");  
	for (int i = 0; i < uid.size; i++)
	{
		if (i > 0) printf(":");
		printf("%02X", mfrc522.uid.uidByte[i]);
	}
	printf("\n");  
}
 
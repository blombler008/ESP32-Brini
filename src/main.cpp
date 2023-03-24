#include <config.hpp>  
 
WiFiUDP udp;
WiFiServer server(8080);

TFT display(TFT_CS, TFT_DC, TFT_LED_PIN, TFT_RST);
ShiftRegister SR(SR_RCK_PIN, SR_CLK_PIN, SR_DATA_PIN); 
MFRC522 mfrc522(RFID_CS, RFID_RST);

TaskHandle_t fs_lua;
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 18, NEO_GRB+NEO_KHZ800);

void printUID(MFRC522::Uid uid);
  
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

void setup() { 
    Serial.begin(115200);  
    //esp_phy_erase_cal_data_in_nvs();
    pixel.begin();  
    //xTaskCreatePinnedToCore(fsinit, "fs_lua", 10000, NULL, 1, &fs_lua, 1); 
    SR.initialise(); 

    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);

    SR.out(0xaa); 
    vTaskDelay(200); 
    SR.out(0x55); 
    #define GAP 8
    #define LINEHIGHT 20 
    #define TEXTHEIGHT 4 
    #define TITLEHEIGHT 16
    #define TITLE "Cocktail-Mixer"
    display.setRotation(Display_Landscape_1);
    display.initialise(&SPI);  
    display.drawHorizontalLine(LINEHIGHT, GAP);  
    display.printTextCentered(TITLE, TITLEHEIGHT);  

	mfrc522.PCD_Init();
	delay(4);
	mfrc522.PCD_DumpVersionToSerial();
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

    vTaskDelay(200);
	SR.out(0);


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

uint8_t data = 0;

void loop() {
    int packetSize = udp.parsePacket();
    
    SR.out(0x01);
    if (packetSize) {
        SR.out(0x03);
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
        SR.out(0x07);	
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
        SR.out(0);	
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
 
#include <config.hpp> 

WiFiUDP udp;
WiFiServer server(8080);
 
MFRC522 mfrc522(RFID_CS, RFID_RST);

void printUID(MFRC522::Uid uid);

void shiftData(uint8_t data) {  
    shiftOut(SR_DATA_PIN, SR_CLK_PIN, LSBFIRST, data);   
	digitalWrite(SR_RCK_PIN, HIGH); 
	digitalWrite(SR_RCK_PIN, LOW);
}


void fsinit() {
    FSWrapper fswrapper;
    fswrapper.begin(FORMAT_LITTLEFS_IF_FAILED);
    fswrapper.listDir("/", 0); 
    
    LuaWrapper lua;
    
    String result = lua.Lua_doFile("/littlefs/test.lua");
    if (!result.isEmpty())
        Serial.println(result);
}

void setup() {
    Serial.begin(115200);  
    
    fsinit(); 

	pinMode(SR_RCK_PIN, OUTPUT);
	pinMode(SR_CLK_PIN, OUTPUT); 
	pinMode(SR_DATA_PIN, OUTPUT);
	digitalWrite(SR_RCK_PIN, LOW);
	digitalWrite(SR_CLK_PIN, LOW);
	digitalWrite(SR_DATA_PIN, LOW); 
    
	shiftData(0xaa);	
    vTaskDelay(200);
	shiftData(0x55);	

    SPI.begin(RFID_SCK, RFID_MISO, RFID_MOSI);
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
    udp.begin(8888);
}

uint8_t data= 0;

void loop() {
    int packetSize = udp.parsePacket();  
    
    shiftData(1);
    if (packetSize) {
         
        shiftData(3);	
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
        shiftData(7);	
		Serial.println("[TCP] Connecting");
		WiFiClient client = server.available(); 
		
		while(client.connected()) {   
			if (client.available()) { 
				String data = client.readString();
				String newData = data.substring(0, data.indexOf("\n"));
				Serial.println("[TCP] Received from Tablet: " + newData); 

                
                Serial.println("[TCP] Sending: Hello Tablet!"); 
                client.print("Hello Tablet!\n");
                client.flush(); 
                Serial.println("[TCP] Send: Hello Tablet!"); 
                Serial.println("[TCP] Closing");   
			}  
		} 
		server.stopAll(); 
		Serial.println("[TCP] Connection Closed"); 
        shiftData(0);	
	}  
    
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
		printUID(mfrc522.uid);
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
 
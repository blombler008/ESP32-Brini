#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <config.hpp> 
 
WiFiUDP udp;
WiFiServer server(8080);
 
void shiftData(uint8_t data) {  
    shiftOut(SR_DATA_PIN, SR_CLK_PIN, LSBFIRST, data);   
	digitalWrite(SR_RCK_PIN, HIGH); 
	digitalWrite(SR_RCK_PIN, LOW);
}

void setup() {
    Serial.begin(115200);

	pinMode(SR_RCK_PIN, OUTPUT);
	pinMode(SR_CLK_PIN, OUTPUT); 
	pinMode(SR_DATA_PIN, OUTPUT);
	digitalWrite(SR_RCK_PIN, LOW);
	digitalWrite(SR_CLK_PIN, LOW);
	digitalWrite(SR_DATA_PIN, LOW); 
    
	shiftData(0xaa);	
    vTaskDelay(200);
	shiftData(0x55);	
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
	


  
}
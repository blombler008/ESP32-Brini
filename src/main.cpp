#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <config.hpp>
 
WiFiUDP udp;
WiFiServer server(8080);
 
void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    server.begin();
	Serial.println("Starting...");
    udp.begin(8888);
}
 
void loop() {
    int packetSize = udp.parsePacket(); 
    if (packetSize) {
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
		Serial.println("Connecting TCP");
		WiFiClient client = server.available(); 
		
		while(client.connected()) {   
			if (client.available()) { 
				String data = client.readString();
				String newData = data.substring(0, data.indexOf("\n"));
				Serial.println("[TCP] Received from Tablet: " + newData); 
				client.print("Hello Tablet!\n");
				client.flush(); 
				Serial.println("[TCP] Send: Hello Tablet!"); 
				Serial.println("[TCP] Closing"); 
				 
			} 
		} 
		server.stopAll(); 
		Serial.println("[TCP] Connection Closed"); 
	}  
	


  
}
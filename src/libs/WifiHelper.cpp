#include "WiFiHelper.hpp"

void WiFiHelper::begin(WiFiHelperConfig_t* wifiConfig) {
    wifi_ssid = wifiConfig->ssid;
    wifi_password = wifiConfig->password;
    wifi_hostname = wifiConfig->hostname;
    udp_port = wifiConfig->udp_port;
    server_port = wifiConfig->server_port; 
    
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    
    server.begin(server_port);
	Serial.println("Starting...");
    udp.setTimeout(20);
    server.setNoDelay(true);
    server.setTimeout(20);
    udp.begin(udp_port);
    
    xTaskCreatePinnedToCore(wifi_loop0, "wifi_loop", 10000, this, 1, &wifi_helper, 0);  
}

void WiFiHelper::sendData(const char* data) {
    xQueueSendToBack(queue, (void*)(&data), 0);
}

void WiFiHelper::setWiFISetupFunction(void (*callback)(WiFiClient* client)) {
    WiFiHelper::WiFISetupFunction = callback;
}

void WiFiHelper::setRecieveCommandFunction(void (*callback)(const char* cmd)) {
    WiFiHelper::commandRecieved = callback;
}

WiFiHelper::WiFiHelper() {
    queue = xQueueCreate(5, sizeof(char*));
}

void runClientSetup(WiFiClient* client, WiFiHelper* helper) {
    while(client->connected()) {   
        if(client->available()) { 
            String data = client->readString();
            String newData = data.substring(0, data.indexOf("\n"));
            Serial.println("[TCP] Received from Tablet: " + newData); 
            client->println("Hello Tablet!");

            helper->WiFISetupFunction(client);
            break;
        }
    }
}

void runClientLoop(WiFiClient* client, WiFiHelper* helper) {
    while(client->connected()) {   
        if (client->available()) { 
            String data = client->readStringUntil('\n');
            String newData = data.substring(0, data.indexOf("\n"));
            Serial.println("[TCP] Received: " + newData); 
            helper->commandRecieved(newData.c_str());
        }
        
        const char* data; 
        if(xQueueReceive(helper->queue, (void*)(&data), 0)) {
            char send[30];
            memcpy(send, data, sizeof(send));
            Serial.printf("Sending: %s\n", send);
            client->println(send); 
        }
    } 
}
void wifi_loop0(void* o) {
    WiFiHelper* helper = (WiFiHelper*)o;
    WiFiUDP udp = helper->udp;
    WiFiServer server = helper->server;
    while(1) {
        int packetSize = udp.parsePacket();

        helper->status = NOTCONNECTED;
        if (packetSize) {
            helper->status = UDPCONNECTED;
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
            helper->status = TCPCONNECTED;
            Serial.println("[TCP] Connecting");
            WiFiClient client = server.available(); 
            runClientSetup(&client, helper);
            
            runClientLoop(&client, helper);
            
            server.stopAll();
            helper->status = NONE;
        }  
    }
    vTaskDelete(helper->wifi_helper);
}
 
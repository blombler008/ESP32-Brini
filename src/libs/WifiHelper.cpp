#include "WiFiHelper.hpp"
void testScan(const char* ssid) {
    Serial.println("Scanning");
    const int16_t n = WiFi.scanNetworks();
    if(n == WIFI_SCAN_FAILED) { 
        Serial.println("scan failed");
        return;
    }
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            // Print SSID and signal strength
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");
}

void WiFiHelper::begin(WiFiHelperConfig_t* wifiConfig) {
    wifi_ssid = wifiConfig->ssid;
    wifi_password = wifiConfig->password;
    wifi_hostname = wifiConfig->hostname;
    udp_port = wifiConfig->udp_port;
    server_port = wifiConfig->server_port; 
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.disconnect();
    WiFi.begin(wifi_ssid, wifi_password); 
    int polTimeout = 0;

    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED && polTimeout < 6) {
        delay(1000);
        polTimeout++;
        Serial.print("...");
    }
    Serial.println("");
    if(polTimeout == 6) { 
        testScan(wifi_ssid);
        Serial.println("Trying SoftAP");
        WiFi.softAP(wifi_ssid, wifi_password);

    }

    server.begin(server_port);
	Serial.println("Starting Server...");
    udp.setTimeout(20);
    server.setNoDelay(true);
    server.setTimeout(20);
    udp.begin(udp_port);
	Serial.println("Server Started!");
    
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
 
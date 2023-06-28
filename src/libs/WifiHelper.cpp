#include "WiFiHelper.hpp"
bool scanNetworks(const char* ssid) {
    logprint("Scanning Networks...");
    bool result = false;
    const int16_t n = WiFi.scanNetworks();
    if(n == WIFI_SCAN_FAILED) { 
        logprint("Scan failed.");
        return result;
    }
    logprint("Scan done.");
    if (n == 0) {
        logprint("No networks found.");
        return result; 
    }
    logprint("%d Networks found.", n);
    for (int i = 0; i < n; ++i) { 
        result = result | WiFi.SSID(i).compareTo(ssid);
        logprint(" - %d: %s (%d)%s", i+1, WiFi.SSID(i).c_str(), WiFi.RSSI(i), (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
        delay(10);
    }  
    return result;
}

void WiFiHelper::begin(WiFiHelperConfig_t* wifiConfig) {
    wifi_ssid = wifiConfig->ssid;
    wifi_password = wifiConfig->password;
    wifi_hostname = wifiConfig->hostname;
    udp_port = wifiConfig->udp_port;
    server_port = wifiConfig->server_port; 
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.disconnect();
    int polTimeout = 0; 
    bool foundNetwork = scanNetworks(wifi_ssid);
    if(foundNetwork) {
        WiFi.begin(wifi_ssid, wifi_password); 
        logprint("Connecting to WiFi...");
        while (WiFi.status() != WL_CONNECTED && polTimeout < 6) {
            delay(1000);
            polTimeout++; 
        } 

    }     
    if(!foundNetwork || polTimeout == 6) { 
        logprint("Opening Access Point: \"%s\", \"%s\"", wifi_ssid, wifi_password);
        WiFi.softAP(wifi_ssid, wifi_password); 
    }

    server.begin(server_port);
	logprint("Starting Server...");
    udp.setTimeout(20);
    server.setNoDelay(true);
    server.setTimeout(20);
    udp.begin(udp_port);
	logprint("Server Started!");
    
    xTaskCreatePinnedToCore(wifi_loop0, "wifi_loop", 10000, this, 1, &wifi_helper, tskNO_AFFINITY);  
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
            logprint("[TCP] Received from Tablet: %s", newData); 
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
            logprint("[TCP] Received: %s", newData); 
            helper->commandRecieved(newData.c_str());
        }
        
        const char* data; 
        if(xQueueReceive(helper->queue, (void*)(&data), 0)) {
            char send[30];
            memcpy(send, data, sizeof(send));
            logprint("Sending: %s", send);
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
            logprint("[UDP] Received UDP message: %s", message);

            udp.beginPacket(udp.remoteIP(), udp.remotePort()); 
            udp.write(WiFi.localIP()[0]); 
            udp.write(WiFi.localIP()[1]); 
            udp.write(WiFi.localIP()[2]); 
            udp.write(WiFi.localIP()[3]); 
            udp.endPacket(); 
            logprint("[UDP] Sent IP: %s", WiFi.localIP().toString()); 
        }

        if(server.hasClient()) {
            helper->status = TCPCONNECTED;
            logprint("[TCP] Connecting");
            WiFiClient client = server.available(); 
            runClientSetup(&client, helper);
            
            runClientLoop(&client, helper);
            
            server.stopAll();
            helper->status = NONE;
        }  
    }
    vTaskDelete(helper->wifi_helper);
}
 
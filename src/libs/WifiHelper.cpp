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

void WiFiHelper::sendData(CMDMessage* msg) {
    xQueueSendToBack(queue, (void*)(msg), 0);
}

WiFiHelper::WiFiHelper() {
    queue = xQueueCreate(5, sizeof(CMDMessage));
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
                  
                CMDMessage data;
                if(xQueueReceive(helper->queue, (void*)(&data), 0)) {
                    Serial.printf("Sending: %s\n", data.data);
                    client.write(data.data);
                    client.flush(); 
                }
            } 
            server.stopAll();
            helper->status = NONE;
        }  
    }
    vTaskDelete(helper->wifi_helper);
}
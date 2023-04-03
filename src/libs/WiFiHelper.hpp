#ifndef WIFI_HELPER_HPP
#define WIFI_HELPER_HPP

#include <WiFi.h> 
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <freertos/queue.h> 

typedef struct {
    const char* ssid;
    const char* password;
    const char* hostname;
    uint16_t udp_port;
    uint16_t server_port; 
} WiFiHelperConfig_t;

typedef enum {
    NONE = 0,
    NOTCONNECTED = 1,
    UDPCONNECTED = 3,
    TCPCONNECTED = 7
} WiFiHelperStates;

typedef struct {
    uint8_t msgid;
    char data[30];
} WiFiCommand;

void wifi_loop0(void* o);

class WiFiHelper {
    private: 
        const char* wifi_ssid;
        const char* wifi_password;
        const char* wifi_hostname;
        uint16_t udp_port;
        uint16_t server_port;
    public:
        QueueHandle_t queue;
        WiFiHelperStates status = NONE;
        WiFiUDP udp;
        WiFiServer server;
        TaskHandle_t wifi_helper; 
        WiFiHelper(); 
        void begin(WiFiHelperConfig_t* wifiConfig);
        WiFiHelperStates getWifiStatus() {return status;};
        void sendData(WiFiCommand* msg);
};



#endif
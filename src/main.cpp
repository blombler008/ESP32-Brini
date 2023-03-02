#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <config.hpp> 
#include <LittleFS.h>
 
#ifndef CONFIG_LITTLEFS_FOR_IDF_3_2
 #include <time.h>
#endif
 
WiFiUDP udp;
WiFiServer server(8080);
 
void shiftData(uint8_t data) {  
    shiftOut(SR_DATA_PIN, SR_CLK_PIN, LSBFIRST, data);   
	digitalWrite(SR_RCK_PIN, HIGH); 
	digitalWrite(SR_RCK_PIN, LOW);
}

#define FORMAT_LITTLEFS_IF_FAILED false

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");

#ifdef CONFIG_LITTLEFS_FOR_IDF_3_2
            Serial.println(file.name());
#else
            Serial.print(file.name());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
#endif

            if(levels){
                listDir(fs, file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");

#ifdef CONFIG_LITTLEFS_FOR_IDF_3_2
            Serial.println(file.size());
#else
            Serial.print(file.size());
            time_t t= file.getLastWrite();
            struct tm * tmstruct = localtime(&t);
            Serial.printf("  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n",(tmstruct->tm_year)+1900,( tmstruct->tm_mon)+1, tmstruct->tm_mday,tmstruct->tm_hour , tmstruct->tm_min, tmstruct->tm_sec);
#endif
        }
        file = root.openNextFile();
    }
}
void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}
void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}
void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.print("- read from file: \"");
    while(file.available()){
        Serial.write(file.read());
    }
    Serial.println("\"");
    file.close();
}
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}
void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}
void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}
void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}
void fsinit() {
    if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        Serial.println("LITTLEFS Mount Failed");
        return;
    }
    
    listDir(LittleFS, "/", 0); 
    LuaWrapper wrapper = LuaWrapper();
    String result = wrapper.Lua_doFile("/test.lua");
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
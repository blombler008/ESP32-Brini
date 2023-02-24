#include "config.hpp" 
#include <SPI.h>
#include <MFRC522.h>
 
MFRC522 mfrc522(RFID_CS, RFID_RST);  // Create MFRC522 instance

void shiftData(uint8_t data) {  
    shiftOut(SR_DATA_PIN, SR_CLK_PIN, LSBFIRST, data);   
	digitalWrite(SR_RCK_PIN, HIGH); 
	digitalWrite(SR_RCK_PIN, LOW);
}

void setup() {
    Serial.begin(SERIAL_BAUD);
    Serial.println();
    Serial.println("Starting Arduino");   
	pinMode(SR_RCK_PIN, OUTPUT);
	pinMode(SR_CLK_PIN, OUTPUT); 
	pinMode(SR_DATA_PIN, OUTPUT);
	digitalWrite(SR_RCK_PIN, LOW);
	digitalWrite(SR_CLK_PIN, LOW);
	digitalWrite(SR_DATA_PIN, LOW);
	shiftData(0x55);
	SPI.begin(RFID_SCK, RFID_MISO, RFID_MOSI);			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
	vTaskDelay(200);
	shiftData(0);
}
uint8_t data= 0;
void loop() {
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial()) {
		data<<=1;
		data++;
		shiftData(data);
		vTaskDelay(500);  
		if(data == 0xff) {
			data = 0;
			shiftData(data);
			vTaskDelay(500);  
		}
		return;
	}
	int i;
	printf("UID OF Device: ");  
	for (i = 0; i < mfrc522.uid.size; i++)
	{
		if (i > 0) printf(":");
		printf("%02X", mfrc522.uid.uidByte[i]);
	}
	printf("\n");  

	// Dump debug info about the card; PICC_HaltA() is automatically called
	//mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}

// void setup() {
//     Serial.begin(SERIAL_BAUD);
//     Serial.println();
//     Serial.println("Starting Arduino");  
//     // shiftOut(SR_DATA_PIN, SR_CLK_PIN, MSBFIRST, 0x55);
//     // vTaskDelay(200); 
//     // shiftOut(SR_DATA_PIN, SR_CLK_PIN, MSBFIRST, 0xaa);  

//     SPI.begin(RFID_SCK, RFID_MISO, RFID_MOSI, RFID_CS);
    
// } 
 
// void loop() {
    
//     vTaskDelay(0); 
    
// } 
 
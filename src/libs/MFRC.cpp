#include "MFRC.hpp"
void card_read_loop(void* o);
Ticker tkCardRead = Ticker();
MFRC::MFRC(uint8_t cs, uint8_t rst, uint32_t timeout) { 
	MFRC::rst = rst;
	MFRC::cs = cs;
	MFRC::timeout = timeout;
	readTimeout = true;
	cardRead = false; 
}
  
void MFRC::begin() {
    mfrc522.PCD_Init(cs, rst);
	mfrc522.PCD_AntennaOn();
	
    xTaskCreatePinnedToCore([](void* o){ ((MFRC*)o)->card_read_loop0(); }, "lua_loop", 10000, this, 1, &MFRC_handle, tskNO_AFFINITY);  
}

void MFRC::PCD_DumpVersionToSerial() {
	Serial.printf(ARDUHAL_LOG_COLOR_I "[%6u][I][%s:%u] %s(): ",(unsigned long) (esp_timer_get_time() / 1000ULL), pathToFileName(__FILE__), __LINE__, __FUNCTION__);
    mfrc522.PCD_DumpVersionToSerial();
	logprint("Scan PICC to see UID, SAK, type, and data blocks..."); 
}

void MFRC::PCD_UIDToSerial(MFRC522::Uid uid) {
	printf("UID OF Device: ");  
	for (int i = 0; i < uid.size; i++) {
		if (i > 0) printf(":");
		printf("%02X", uid.uidByte[i]);
	}
	printf("\n");  
}

String MFRC::PCD_UIDToString(MFRC522::Uid uid) {
	String str;
	char buff[12];
	for (int i = 0; i < uid.size; i++) {
		if (i > 0) str.concat(':'); 
		sprintf(buff, "%02X", uid.uidByte[i]);
		str.concat(buff);
	}
	return str;
}

bool MFRC::hasReadCard() {
	
	if(!isReadTimeout()) {
		return false;
	}

	if(cardRead) {
		cardRead = false;
		return true;
	}

	return false;
}

bool MFRC::isReadTimeout() { 
	//Serial.printf("%i\n", readTimeout);
    return esp_timer_get_time() < timeoutEnd;
}

MFRC522::Uid MFRC::getUID() {
    return uid;
}

void MFRC::card_read_loop0() {
	while(1) {
		vTaskDelay(10 / portTICK_PERIOD_MS);
		if(isReadTimeout()) {
			portYIELD();
			continue;
		} 
		if(!mfrc522.PICC_IsNewCardPresent()) {
			continue;
		}
		logprint("New keycard");
		if (!mfrc522.PICC_ReadCardSerial()) {
			continue;
		}
		uid = mfrc522.uid;  
		logprint("Card read: %s", PCD_UIDToString(uid));
		timeoutEnd = esp_timer_get_time() + 1000 * timeout;
		readTimeout = true;
		cardRead = true;
		mfrc522.PCD_StopCrypto1(); 
	}
	 
} 
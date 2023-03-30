#include "MFRC.hpp"

MFRC::MFRC(uint8_t cs, uint8_t rst, uint32_t timeout) { 
	MFRC::rst = rst;
	MFRC::cs = cs;
	MFRC::timeout = timeout;
}
  
void MFRC::begin()
{
    mfrc522.PCD_Init(cs, rst);
	mfrc522.PCD_AntennaOn(); 
	
}

void MFRC::PCD_DumpVersionToSerial() {
    mfrc522.PCD_DumpVersionToSerial();
	Serial.println("Scan PICC to see UID, SAK, type, and data blocks..."); 
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
	char buff[3];
	for (int i = 0; i < uid.size; i++) {
		if (i > 0) str.concat(':'); 
		sprintf(buff, "%02X", uid.uidByte[i]);
		str.concat(buff);
	}
	return str;
}

bool MFRC::hasReadCard() {
	
	if(isReadTimeout()) 
		return false;

	if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
		mfrc522.PCD_StopCrypto1();
		readTimeout = true; 
		timeoutEnd = esp_timer_get_time() + timeout*1000;
		uid = mfrc522.uid;
		return true;
	} 

	return false;
}

bool MFRC::isReadTimeout() { 
	//Serial.printf("%i\n", readTimeout);
	if(esp_timer_get_time() > timeoutEnd) {
		readTimeout = false;
	}
    return readTimeout;
}

MFRC522::Uid MFRC::getUID() {
    return uid;
}

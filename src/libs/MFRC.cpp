#include "MFRC.hpp"

MFRC::MFRC(uint8_t cs, uint8_t rst) { 
	MFRC::rst = rst;
	MFRC::cs = cs;
}

void MFRC::begin() {
	mfrc522.PCD_Init(cs, rst);
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

void MFRC::PCD_UIDToString(char* buffer, MFRC522::Uid uid) {
	String str;
	char buff[3];
	for (int i = 0; i < uid.size; i++) {
		if (i > 0) str.concat(':'); 
		sprintf(buff, "%02X", uid.uidByte[i]);
		str.concat(buff);
	}
	(*buffer) = *(str.c_str());
}
 
 
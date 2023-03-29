#include "MFRC.hpp"

void MFRC::begin(uint8_t cs, uint8_t rst) {
	mfrc522.PCD_Init(cs, rst);
}

void MFRC::PCD_DumpVersionToSerial() {
    mfrc522.PCD_DumpVersionToSerial();
}

void MFRC::PCD_UIDToSerial(MFRC522::Uid uid) { 
	printf("UID OF Device: ");  
	for (int i = 0; i < uid.size; i++)
	{
		if (i > 0) printf(":");
		printf("%02X", mfrc522.uid.uidByte[i]);
	}
	printf("\n");  
}
 
#ifndef MFRC_HPP
#define MFRC_HPP

#include <MFRC522.h>

class MFRC {
    private:
        MFRC522 mfrc522;
        uint8_t cs;
        uint8_t rst;
    public:
        MFRC(uint8_t cs, uint8_t rst);
        void begin();
        void PCD_DumpVersionToSerial();
        void PCD_UIDToSerial(MFRC522::Uid uid);
        void PCD_UIDToString(char* buffer, MFRC522::Uid uid);
}; 


#endif
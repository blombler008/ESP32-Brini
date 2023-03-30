#ifndef MFRC_HPP
#define MFRC_HPP

#include <MFRC522.h>

class MFRC {
    private:
        uint8_t cs;
        uint8_t rst;
        bool readTimeout;
        uint64_t timeoutEnd;
        uint32_t timeout;
        MFRC522::Uid uid;
    public:
        MFRC522 mfrc522;
        MFRC(uint8_t cs, uint8_t rst, uint32_t timeout = 10000);
        void begin();
        void PCD_DumpVersionToSerial();
        void PCD_UIDToSerial(MFRC522::Uid uid);
        String PCD_UIDToString(MFRC522::Uid uid);
        bool hasReadCard();
        bool isReadTimeout();
        MFRC522::Uid getUID();
}; 


#endif
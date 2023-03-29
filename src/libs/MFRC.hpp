#ifndef MFRC_HPP
#define MFRC_HPP

#include <MFRC522.h>

class MFRC {
    private:
        MFRC522 mfrc522;
        uint8_t cs;
        uint8_t rst;
    public:
        MFRC();
        void begin(uint8_t cs, uint8_t rst);
        void PCD_DumpVersionToSerial();
        void PCD_UIDToSerial(MFRC522::Uid uid);
}; 


#endif
#ifndef FS_WRAPPER_HPP
#define FS_WRAPPER_HPP
#include <FS.h>
#include <SD.h>
#include <LittleFS.h> 
#include "PrintHelper.hpp"

class FSWrapper {
    public:   
        FSWrapper(int chip_select = SS);   
        void begin(bool formatOnFail = false); 
        void listDir(const char * dirname, uint8_t levels);
        void createDir(const char * path);
        void removeDir(const char * path);
        String readFile(const char * path);
        void writeFile(const char * path, const char * message);
        void appendFile(const char * path, const char * message);
        void renameFile(const char * path1, const char * path2);
        void deleteFile(const char * path);
        bool isLittleFSMounted();
        bool isSDMounted();

    private:
        int sd_ss= 0;
        FS &fs = LittleFS;
        bool littleFSmounted = false;
        bool sdFSmounted = false;
};

#endif

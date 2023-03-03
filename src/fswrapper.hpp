#ifndef FS_WRAPPER_HPP
#define FS_WRAPPER_HPP
#include <FS.h>
#include <LittleFS.h>

class FSWrapper {
    public:  
        FSWrapper();   
        void begin(bool formatOnFail = false); 
        void listDir(const char * dirname, uint8_t levels);
        void createDir(const char * path);
        void removeDir(const char * path);
        String readFile(const char * path);
        void writeFile(const char * path, const char * message);
        void appendFile(const char * path, const char * message);
        void renameFile(const char * path1, const char * path2);
        void deleteFile(const char * path);
        void isMounted();

    private:
        FS &fs = LittleFS;
        bool mounted = false;
};

#endif
#include <fswrapper.hpp>
#include <LittleFS.h>
#include "PrintHelper.hpp"
 
FSWrapper::FSWrapper() {}

bool openFile(File* filef, FS &fs, const char* path, const char* mode = FILE_READ);
void printLastWrite(File* file);
void printFileInfo(FSWrapper* w, File *file, int levels);

void FSWrapper::begin(bool formatOnFail) {
    if(!LittleFS.begin(formatOnFail)){
        Serial.println("LittleFS Mount Failed");
        return;
    }
    mounted = true;
}

void FSWrapper::listDir(const char * dirname, uint8_t levels){

    if (!mounted) return;

    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);

    if(!root){
        Serial.println("- failed to open directory");
        return;
    }

    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){ 
        printFileInfo(this, &file, levels);
        file = root.openNextFile();
    }
}

void FSWrapper::createDir(const char * path){
    
    if (!mounted) return; 

    if(fs.mkdir(path)) {
        dprintf("Dir %s created!\n", path); 
        return;
    } 
         
    Serial.println("mkdir failed");
    
}

void FSWrapper::removeDir(const char * path){
    
    if (!mounted) return; 

    if(fs.rmdir(path)) {
        dprintf("Dir %s removed!\n", path);   
        return;
    }
    
    Serial.println("rmdir failed"); 
}

String FSWrapper::readFile(const char * path){
    
    if (!mounted) return emptyString;
    dprintf("Reading file: %s\r\n", path);

    File file; 
    if(!openFile(&file, fs, path)) return emptyString;
 
    String buf;
    while(file.available()){
        buf.concat(file.read());
    } 

    file.close();
    return buf;
}

void FSWrapper::writeFile(const char* path, const char* message){
    
    if (!mounted) return;
    
    dprintf("Writing file: %s\r\n", path);

    File file;  
    if(!openFile(&file, fs, path, FILE_WRITE))  return;

    bool writeSuccess = file.print(message); 
    dprintf("- file written: %s : %s\r\n", path, writeSuccess);

    file.close();
}

void FSWrapper::appendFile(const char* path, const char* message){
    
    if (!mounted) return;
    dprintf("Appending to file: %s\r\n", path);
 
    File file; 
    if(!openFile(&file, fs, path, FILE_APPEND)) return;

    bool writeSuccess = file.print(message); 
    dprintf("- file append: %s : %s\r\n", path, writeSuccess);

    file.close();
}

void FSWrapper::renameFile(const char* path1, const char* path2){
    
    if (!mounted) return; 

    if (!fs.rename(path1, path2)) Serial.printf("Rename %s to %s failed\r\n", path1, path2);
    
}

void FSWrapper::deleteFile(const char* path){
    
    if (!mounted) return; 

    if(!fs.remove(path)) Serial.printf("delete %s failed", path);  
} 

bool openFile(fs::File* filef, fs::FS &fs, const char* path, const char* mode) {
    fs::File file = fs.open(path, mode, false);
    
    if(!file || file.isDirectory()){
        Serial.printf("- failed to open file: %s\r\n", path); 
        return false;
    }

    (*filef) = file;
    return true;
}

void printLastWrite(File* file) {
    const char* format = "  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n";
    time_t t= file->getLastWrite(); 
    struct tm * tmstruct = localtime(&t);

    int year = (tmstruct->tm_year)+1900;
    int mon = (tmstruct->tm_mon)+1;
    int day = tmstruct->tm_mday;
    int hour = tmstruct->tm_hour;
    int min = tmstruct->tm_min;
    int sec = tmstruct->tm_sec;


    Serial.printf(format, year, mon, day, hour, min, sec);
    
}

void printFileInfo(FSWrapper* w, File* file, int levels) {
    if(file->isDirectory()){
        Serial.print("  DIR : ");
        Serial.print(file->name());
    
        printLastWrite(file);
        if(levels) w->listDir(file->name(), levels -1);
        return; 
    }
    Serial.print("  FILE: ");
    Serial.print(file->name());
    Serial.print("  SIZE: ");

    Serial.print(file->size()); 
    printLastWrite(file);
}

#include "FSWrapper.hpp"
 
FSWrapper::FSWrapper() {}

bool openFile(File* filef, FS &fs, const char* path, const char* mode = FILE_READ);
void printLastWrite(File* file);
void printFileInfo(FSWrapper* w, File *file, int levels);

void FSWrapper::begin(bool formatOnFail) {
    if(!LittleFS.begin(formatOnFail)){
        printf("LittleFS Mount Failed\n");
        return;
    }
    mounted = true;
}

void FSWrapper::listDir(const char * dirname, uint8_t levels){

    if (!mounted) return;

    printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);

    if(!root){
        printf("- failed to open directory\n");
        return;
    }

    if(!root.isDirectory()){
        printf(" - not a directory\n");
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
         
    printf("mkdir failed\n");
    
}

void FSWrapper::removeDir(const char * path){
    
    if (!mounted) return; 

    if(fs.rmdir(path)) {
        dprintf("Dir %s removed!\n", path);   
        return;
    }
    
    printf("rmdir failed\n"); 
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

    if (!fs.rename(path1, path2)) printf("Rename %s to %s failed\r\n", path1, path2);
    
}

void FSWrapper::deleteFile(const char* path){
    
    if (!mounted) return; 

    if(!fs.remove(path)) printf("delete %s failed", path);  
} 

bool openFile(fs::File* filef, fs::FS &fs, const char* path, const char* mode) {
    fs::File file = fs.open(path, mode, false);
    
    if(!file || file.isDirectory()){
        printf("- failed to open file: %s\r\n", path); 
        return false;
    }

    (*filef) = file;
    return true;
}

void printLastWrite(File* file) {
    const char* format = "  LAST WRITE: %d-%02d-%02d %02d:%02d:%02d\n";
    time_t t= file->getLastWrite(); 
    String time = printTimeToString(format, t);
     
    printf(time.c_str());
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

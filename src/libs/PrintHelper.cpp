#include "PrintHelper.hpp"
 
String printTimeToString(const char* format, time_t t) {
    struct tm * tmstruct = localtime(&t);

    int year = (tmstruct->tm_year)+1900;
    int mon = (tmstruct->tm_mon)+1;
    int day = tmstruct->tm_mday;
    int hour = tmstruct->tm_hour;
    int min = tmstruct->tm_min;
    int sec = tmstruct->tm_sec;
    int size = strlen(format);
    char buf[size];
    sprintf((char*)&buf, format, year, mon, day, hour, min, sec);
    return String((char*)&buf);
}
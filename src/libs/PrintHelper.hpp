#ifndef PRINT_HELP_HPP
#define PRINT_HELP_HPP
#include <time.h>
#include <FreeRTOS.h>
#include <string.h>
#include <WString.h>

#define logprint(format, ...) log_i(format, ##__VA_ARGS__)

#ifdef DEBUG

#define dprintf(msg, ...) Serial.printf(msg, ##__VA_ARGS__);
#define dprintln(msg) Serial.println(msg);
#define dprint(msg) Serial.print(msg);
            
#else

#define dprintf(msg, ...)
#define dprintln(msg) 
#define dprint(msg)

#endif
 
String printTimeToString(const char* format, time_t time);

#endif
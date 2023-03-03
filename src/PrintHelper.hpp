#ifndef PRINT_HELP_HPP
#define PRINT_HELP_HPP

#ifdef DEBUG

#define dprintf(msg, ...) Serial.printf(msg, __VA_ARGS__);
#define dprintln(msg) Serial.println(msg);
#define dprint(msg) Serial.print(msg);
            
#else

#define dprintf(msg, ...)
#define dprintln(msg) 
#define dprint(msg)

#endif
#endif
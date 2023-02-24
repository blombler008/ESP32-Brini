#ifndef PRINT_HELP_HPP
#define PRINT_HELP_HPP
#define dprintf(msg, ...) \
            do { if (DEBUG) Serial.printf(msg, __VA_ARGS__); } while (0) 
#define dprintln(msg) \
            do { if (DEBUG) Serial.println(msg); } while (0)
#define dprint(msg) \
            do { if (DEBUG) Serial.print(msg); } while (0)
#endif
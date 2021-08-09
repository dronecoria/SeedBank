#ifndef DEFINES_H
#define DEFINES_H

#define DEBUG_SERIAL
#ifdef DEBUG_SERIAL
    #define SERIAL_PRINT(arg) Serial.print(arg);
    #define SERIAL_PRINTLN(arg) Serial.println(arg);
#else
    #define SERIAL_PRINT(arg)
    #define SERIAL_PRINTLN(arg)
#endif

#define CONFIG_FILENAME "/config.json"

#define TEMP_ERROR_READING -1000.0

#endif

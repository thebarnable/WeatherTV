#pragma once

#include "Adafruit_SSD1306.h"
#include "WString.h"
#include "HardwareSerial.h"

class Printer {
public:
    Printer(Adafruit_SSD1306* display) : display(display), to_display(false), to_serial(true) {

    }
    Printer() : display(nullptr), to_display(false), to_serial(true) {

    }

    void println(const String &s) {
        if(to_display)
            display->println(s);
        if(to_serial)
            Serial.println(s);
    }

    void printf(const char* fmt, ...) {
        char buf[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        println(String(buf));
    }

    bool to_display;
    bool to_serial;
private:
    Adafruit_SSD1306* display;
};
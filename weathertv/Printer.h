#pragma once

#include "Adafruit_SSD1306.h"
#include "WString.h"
#include "HardwareSerial.h"

class Printer {
public:
    Printer(Adafruit_SSD1306* display) : display(display) {

    }

    void println(const String &s) {
        if(println_DISPLAY)
            display->println(s);
        if(println_SERIAL)
            Serial.println(s);
    }

    bool println_DISPLAY = false;
    bool println_SERIAL = true;
private:
    Adafruit_SSD1306* display;
};
#pragma once

/*void debugSerial(const String& msg) {
  Serial.println(F(msg));
}
void debugDisplay(const String&)*/

#ifdef DEBUG_SERIAL
  void debugSerial(String msg) {
    Serial.println(msg);
  }
#else
  void debugSerial(String msg) {
    // do nothing
  }
#endif

#ifdef DEBUG_DISPLAY
  void debugDisplay(String msg) {
    display.println(msg); display.display();
  }
#else
  void debugDisplay(String msg) {
    // do nothing
  }
#endif

#define DEBUG(s) {debugSerial(s); debugDisplay(s);}

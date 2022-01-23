#include "ESP8266WiFi.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>

#include "wifi.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// SSD1306 display (on Arduino UNO/ESP8266): A4(SDA), A5(SCL))
#define OLED_RESET     -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wifi Data
HTTPClient http;
String jsonBuffer;

bool madeRequest=false;

void setup() {
  Serial.begin(9600);

  // setup ssd1306 display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    Serial.println(F("SSD1306 allocation failed"));
    while(1); // don't proceed, loop forever
  }

  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0,0); // Start at top-left corner
  display.cp437(true); // Use full 256 char 'Code Page 437' font
  display.clearDisplay();display.display();
  Serial.println(F("Booting WeatherTV"));
  display.println(F("Booting WeatherTV"));

  // setup Wifi
  Serial.println(F("> Connecting to WiFi..."));
  display.println(F("> Connecting to WiFi...")); display.display();
  WiFi.begin(wifiSSID, wifiPW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.write('.');
  }
  Serial.println(F("Wifi connected"));
  display.println(F("Wifi connected")); display.display();
}

void loop() {
  delay(5000);
  Serial.println(F("still alive"));
  /*
  if(!madeRequest) {
    WiFiClient client;
    if(http.begin(client, openWeatherMapAPI.c_str())) {
      Serial.print("[HTTP] GET...\n");
      int httpCode = http.GET();
      if(httpCode > 0) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          Serial.println(http.getString());
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
    } {
      Serial.print("[HTTP] Couldn't connect\n");
    }
    madeRequest= true;
  }*/
}

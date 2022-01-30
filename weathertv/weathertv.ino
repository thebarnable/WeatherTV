#include "ESP8266WiFi.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "wifi.h"

// SSD1306 display (on Arduino UNO/ESP8266): A4(SDA), A5(SCL))
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Debugging to serial and OLED display (needs to know 'display')
#define DEBUG_SERIAL
#define DEBUG_DISPLAY
#include "debug.h"

// WiFi
HTTPClient http;

bool madeRequest=false;
WiFiClient client;
int httpCode;

// Weather Data
/* Example:
{"coord":{"lon":6.0834,"lat":50.7766},
 "weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04d"}],
 "base":"stations",
 "main":{"temp":273.3,"feels_like":268.64,"temp_min":272.55,"temp_max":274.23,"pressure":1026,"humidity":79},
 "visibility":10000,
 "wind":{"speed":4.63,"deg":243,"gust":6.21},
 "clouds":{"all":80},"dt":1674991923,
 "sys":{"type":2,"id":2013497,"country":"DE","sunrise":1674976593,"sunset":1675009241},
 "timezone":3600,
 "id":3247449,
 "name":"Aachen",
 "cod":200}
*/
StaticJsonDocument<1000> doc;

void setup() {
  Serial.begin(9600);

  // setup ssd1306 display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    DEBUG("ERROR: SSD1306 allocation failed"); // TODO: use F("")?
    while(1); // don't proceed, loop forever
  }

  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0,0); // Start at top-left corner
  display.cp437(true); // Use full 256 char 'Code Page 437' font
  display.clearDisplay();display.display();
  DEBUG("Booting WeatherTV");

  // setup Wifi
  DEBUG("> Connecting to WiFi");
  WiFi.begin(wifiSSID, wifiPW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.write('.'); display.display();
  }
  DEBUG("> WiFi connection successful");

  delay(500);
}

void loop() {
  delay(5000);
  DEBUG("still alive");

  if(!madeRequest) {
    if(http.begin(client, openWeatherMapAPI.c_str())) {
      Serial.print("[HTTP] GET...\n");
      httpCode = http.GET();
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
  } else {
    if(httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if(httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        deserializeJson(doc, http.getString().c_str());
        serializeJson(doc["weather"], Serial);

        float lon = doc["coord"]["lon"];
        float lat = doc["coord"]["lat"];

        Serial.printf("longitude: %.4f; latitude: %.4f\n", lon, lat);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  }
}

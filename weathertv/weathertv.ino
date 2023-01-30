#include "ESP8266WiFi.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include "Printer.h"
#include "wifi.h"

// SSD1306 display (on Arduino UNO/ESP8266): A4(SDA), A5(SCL))
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// printer.printlnging to serial and OLED display (needs to know 'display')
//bool printer.println_DISPLAY = true;
//bool printer.println_SERIAL = true;

// WiFi
HTTPClient http;

bool madeRequest=false;
WiFiClient client;
int httpCode;

// GPIO
constexpr uint8_t BUTTON = 16; // D0 == GPIO16

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
Printer printer(&display);

// State
enum DisplayState {
  DEBUG,
  MAIN,
  STAT
};
DisplayState displayState = DEBUG;
int prevButtonState = LOW;

bool getWeatherData(StaticJsonDocument<1000>& weatherDoc) {
  printer.println("[getWeatherData] Connecting to openweathermap...");
  if(http.begin(client, openWeatherMapAPI.c_str())) {
    printer.println("[getWeatherData] Connected");
  } else {
    printer.println("[getWeatherData] Couldn't connect");
    return false;
  }

  int httpCode = 0;
  while(httpCode != HTTP_CODE_OK) { // TODO: || HTTP_CODE_MOVED_PERMANENTLY
    printer.println("[getWeatherData] Sending GET request...");
    httpCode = http.GET();
    if(httpCode == HTTP_CODE_OK) { 
      printer.println("[getWeatherData] Got HTTP_CODE_OK :)");
      deserializeJson(weatherDoc, http.getString().c_str());
      //serializeJson(weatherDoc["weather"], Serial);

      /*float lon = weatherDoc["coord"]["lon"];
      float lat = weatherDoc["coord"]["lat"];

      printer.printf("longitude: %.4f; latitude: %.4f\n", lon, lat);*/
    } else {
      printer.printf("[getWeatherData] Didn't get HTTP_CODE_OK :( \n >Error: %s\n", http.errorToString(httpCode).c_str());
      return false;
    }
  }
  return true;
}

void displayMain() {
  display.clearDisplay();
  display.setCursor(0,0);

  // Date
  String exampleDate = "Tue, Jan 29";
  uint32_t dateLength = exampleDate.length();
  Serial.printf("datelength: %d", dateLength);
  display.setTextSize(1);
  display.setCursor(64-dateLength/2*4,0);
  display.print(exampleDate);

  // WiFi connection

  // Weather icon

  // Temperature
  String exampleTemp = "28";
  display.setTextSize(2);
  display.setCursor(64-8,16);
  display.print(exampleTemp);
  display.drawCircle(4, 4, 4, 0xFFFFu);

  // Place

  // Description

  // Sun rise/fall

  display.display();
}

void displayStat() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("stat");
  display.display();
}

void displayDebug() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("debug");
  display.display();
}

void setup() {
  Serial.begin(9600);

  // setup ssd1306 display
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    Serial.println("ERROR: SSD1306 allocation failed"); // TODO: use F("")?
    while(1); // don't proceed, loop forever
  }

  display.setTextSize(1); // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0,0); // Start at top-left corner
  display.cp437(true); // Use full 256 char 'Code Page 437' font
  display.clearDisplay();
  display.display();
  printer.turnOnDisplay();
  printer.println("Booting WeatherTV");

  // setup Wifi
  printer.println("> Connecting to WiFi");
  WiFi.begin(wifiSSID, wifiPW);
  while (WiFi.status() != WL_CONNECTED)
    delay(500);
  printer.println("> WiFi connection successful");

  while(1) {
    delay(100);
    int buttonState = digitalRead(BUTTON);
    if(buttonState == HIGH)
      break;
  }

  // setup GPIOs
  pinMode(BUTTON, INPUT);
}

void loop() {
  // button press -> change display
  delay(100);
  int buttonState = digitalRead(BUTTON);

  if(buttonState == HIGH && prevButtonState == LOW) { // low -> high = button just pressed
    switch(displayState) {
      case DEBUG: displayState = MAIN;  Serial.println("debug -> main"); break;
      case MAIN:  displayState = STAT;  Serial.println("main -> stat"); break;
      case STAT:  displayState = DEBUG; Serial.println("stat -> debug"); break;
    }

    switch(displayState) {
      case DEBUG: displayDebug(); break;
      case MAIN:  displayMain(); break;
      case STAT:  displayStat(); break;
    }
  }

  prevButtonState = buttonState;
}

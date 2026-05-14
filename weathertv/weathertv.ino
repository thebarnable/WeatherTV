#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>
//#include "esp_http_client.h"
#include <ArduinoJson.h>

#include "Printer.h"
#include "wifi.h"

// SSD1306 display (on Arduino UNO/ESP8266): A4(SDA), A5(SCL))
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1   // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// functionality toggles (for debugging)
constexpr bool BUTTONS_ACTIVE   = false;
constexpr bool WIFI_ACTIVE      = true;
constexpr bool DISPLAY_ACTIVE   = false;
constexpr bool WAIT_FOR_SERIAL  = true;

// WiFi
HTTPClient http;

bool madeRequest=false;
WiFiClient client;
int httpCode;

// GPIO
constexpr uint8_t BUTTON = D10;

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
  SUN,
  TEMP,
  DATE
};
DisplayState displayState = SUN;
int prevButtonState = HIGH;

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
      printer.println("[getWeatherData] Didn't get HTTP_CODE_OK :( ");
      //printer.printf("[getWeatherData] Didn't get HTTP_CODE_OK :( \n >Error: %s\n", http.errorToString(httpCode).c_str());
      return false;
    }
  }
  return true;
}

void displaySun() {
  if(DISPLAY_ACTIVE) {
    display.clearDisplay();
    display.setCursor(0,0);

    String icon = "sun";
    display.setTextSize(4);
    display.setCursor(64-8,16);
    display.print(icon);

    display.display();
  }
}

void displayTemp() {
  if(DISPLAY_ACTIVE) {
    display.clearDisplay();
    display.setCursor(0,0);

    String exampleTemp = "28";
    display.setTextSize(2);
    display.setCursor(64-8,16);
    display.print(exampleTemp);
    display.write(0xF8); // degrees symbol

    display.display();
  }
}

void displayDate() {
  if(DISPLAY_ACTIVE) {
    display.clearDisplay();
    display.setCursor(0,0);
  
    String exampleDate = "Tue, Jan 29";
    uint32_t dateLength = exampleDate.length();
    printer.printf("datelength: %d\n", dateLength);
    display.setTextSize(2);
    display.setCursor(64-dateLength/2*4,16);
    display.print(exampleDate);
  
    display.display();
  }
}

void setup() {
  // setup serial connection
  Serial.begin(9600);
  printer.to_serial = true;
  
  // if wanted, wait for user to establish connection and send any data (i.e. press any button)
  if(WAIT_FOR_SERIAL) {
    delay(1000);

    while(!Serial.available()) {}
    Serial.println("Got connection!");
  }

  // setup GPIOs
  if(BUTTONS_ACTIVE) {
    pinMode(BUTTON, INPUT_PULLUP);
  }

  // setup ssd1306 display
  if(DISPLAY_ACTIVE) {
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
      printer.println("ERROR: SSD1306 allocation failed"); // TODO: use F("")?
      while(1); // don't proceed, loop forever
    }

    display.setTextSize(1); // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0,0); // Start at top-left corner
    display.cp437(true); // Use full 256 char 'Code Page 437' font
    display.clearDisplay();
    display.display();

    printer.to_display = true;
  }

  printer.println("Booting WeatherTV");

  // setup Wifi
  if(WIFI_ACTIVE) {
    printer.println("> Connecting to WiFi");
    WiFi.begin(wifiSSID, wifiPW);
    while (WiFi.status() != WL_CONNECTED)
      delay(500);
    printer.println("> WiFi connection successful");
  }
}

void loop() {
  // button press or Enter via Serial connection -> change display
  delay(100);
  int buttonState;
  bool stateChange = false;
  if(BUTTONS_ACTIVE) {
    buttonState = digitalRead(BUTTON);
    stateChange = buttonState == LOW && prevButtonState == HIGH;
  }

  while(Serial.available()) {
    int c = Serial.read();
    stateChange = c == '\n' || c == '\r';
  }

  if(stateChange) { // high -> low = button just pressed
    switch(displayState) {
      case SUN: displayState = TEMP; printer.println("Switching states: SUN -> TEMP"); break;
      case TEMP: displayState = DATE;  printer.println("Switching state: TEMP -> DATE"); break;
      case DATE: displayState = SUN; printer.println("Switching state: DATE -> SUN"); break;
    }

    switch(displayState) {
      case SUN: displaySun(); break;
      case TEMP: displayTemp(); break;
      case DATE: displayDate(); break;
    }
  }

  prevButtonState = buttonState;
}

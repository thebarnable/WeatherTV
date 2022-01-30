# WeatherTV

### Setup
```
arduino-cli core update-index # update core libs for this project config
arduino-cli core install esp8266:esp8266
arduino-cli lib install "Adafruit SSD1306"
arduino-cli lib install "ArduinoJson"
cd weathertv && arduino-cli board attach esp8266:esp8266:nodemcuv2 # optional: creates sketch.json, no need to specify --fqpn anymore

echo "String wifiSSID = \"Insert WiFi SSID\"; String wifiPW = \"Insert WiFi password\"; String openWeatherMapAPI = \"Insert openWeatherMap API token\";" > wifi.h
```

### Compile
```
arduino-cli compile [--fqbn esp8266:esp8266:nodemcuv2]
arduino-cli upload -p /dev/ttyUSB0  [--fqbn esp8266:esp8266:nodemcuv2]
```

### Connect
```
minicom -D /dev/ttyUSB0 -b 9600
```

### Library Locations
* /usr/share/arduino
* <repo-dir>/.arduino15/user/libraries

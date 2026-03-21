# WeatherTV

### Setup
```
arduino-cli --config-file arduino-cli.yaml config add board_manager.additional_urls https://arduino.esp8266.com/stable/package_esp8266com_index.json
arduino-cli --config-file arduino-cli.yaml core update-index # update core libs for this project config
arduino-cli --config-file arduino-cli.yaml core install esp8266:esp8266
arduino-cli --config-file arduino-cli.yaml lib install "Adafruit SSD1306"
arduino-cli --config-file arduino-cli.yaml lib install "ArduinoJson"

echo "String wifiSSID = \"Insert WiFi SSID\"; String wifiPW = \"Insert WiFi password\"; String openWeatherMapAPI = \"Insert openWeatherMap API token\";" > wifi.h
```

### Compile
```
arduino-cli compile --config-file arduino-cli.yaml --fqbn esp8266:esp8266:generic weathertv/
arduino-cli upload -p /dev/ttyUSB0  --fqbn esp8266:esp8266:nodemcuv2
```

### Connect
```
minicom -D /dev/ttyUSB0 -b 9600
```

### Library Locations
* /usr/share/arduino
* <repo-dir>/.arduino15/user/libraries

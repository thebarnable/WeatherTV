# WeatherTV

### Setup
```
arduino-cli core update-index # update core libs for this project config
arduino-cli core install esp8266:esp8266
arduino-cli lib install "Adafruit SSD1306"
cd weathertv && arduino-cli board attach esp8266:esp8266:nodemcuv2 # optional: creates sketch.json, no need to specify --fqpn anymore
```

### Compile
```
arduino-cli compile [--fqbn esp8266:esp8266:nodemcuv2]
arduino-cli upload -p /dev/ttyUSB0  [--fqbn esp8266:esp8266:nodemcuv2]
```

### Library Locations
* /usr/share/arduino
* <repo-dir>/.arduino15/user/libraries

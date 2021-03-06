# Arduino_Pool_Control_MQTT
Control the two pumps (filter and solar heater) from an MQTT app. 

I ave a pool with two pumps in parallel: 1 to filter the water and one to pump water through solar heating panels. The aim of this project is to control the pumps with relays through an MQTT client app. 

Since the pump is far away from any internet connection, I used radio wave (HC12) from the arduino to an ESP8266 which is connected to internet.

Required:
- ESP8266 https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/ to configure Arduino IDE. Arduino + ESP8266 board -- use these switches https://createlabz.store/products/arduino-uno-wifi-r3-compatible-atmega328p-esp8266 or https://forum.arduino.cc/t/atmega328p-esp8266-wifi-tutorial/944380/2
- alternatively, a ESP32 can be used (IDE setup: https://www.instructables.com/How-to-Set-Up-WeMos-TTgo-ESP32-Uno-D1-R32/)
- Relay
- MQTT broker
- MQTT Client APP

Optional for long distance:
- Arduino
- 2x HC-12 Wireless Transceiver Module

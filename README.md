# Arduino_Pool_Control_MQTT
Control the two pumps (filter and solar heater) from an MQTT app. 

I ave a pool with two pumps in parallel: 1 to filter the water and one to pump water through solar heating panels. The aim of this project is to control the pumps with relays through an MQTT client app. 

Since the pump is far away from any internet connection, I used radio wave from one arduino to another arduino which is connected to internet.

Required:
- Arduino UNO or other
- Ethernet shield for arduino (or wifi shield)
- Relay
- MQTT broker
- MQTT Client APP

Optional for long distance:
- Second Arduino
- 2x HC-12 Wireless Transceiver Module

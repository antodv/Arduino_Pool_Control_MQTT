/*
  Project by A. De Vestel
  Code used: https://howtomechatronics.com/tutorials/arduino/arduino-and-hc-12-long-range-wireless-communication-module/
 Basic ESP8266 MQTT example
 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off
 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"
*/

#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Variables
  bool automatic = 0;
  int solarPanelPump = 0;
  int filterPump = 0;
  float temperatureIn = 0;
  float temperatureOut = 0;

//Serial communication
  SoftwareSerial HC12(10, 11);         // HC-12 TX Pin, HC-12 RX Pin
  byte incomingByte;
  String HC12readBuffer = "";

//WIFI
  const char* ssid = "X20";
  const char* password = "Passw03%";
//MQTT
  long lastReconnectAttempt = 0;
  const char* mqtt_server = "broker.mqtt-dashboard.com";
  WiFiClient espClient;
  PubSubClient client(espClient);
  unsigned long lastMsg = 0;
  #define MSG_BUFFER_SIZE  (50)
  char msg[MSG_BUFFER_SIZE];
  int value = 0;


void setup() {
  //Serial
    Serial.begin(9600);                   // Open serial port to computer
  //HC12
    HC12.begin(9600);                     // Open serial port to HC12
  //MQTT
    pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() { 
  //radio communication
    if (HC12.available()) {
      readHC12();
    }

  //MQTT code
    if (!client.connected()) {
        long now = millis();
        if (now - lastReconnectAttempt > 5000) {
          lastReconnectAttempt = now;
          // Attempt to reconnect
          if (reconnect()) {
            lastReconnectAttempt = 0;
            } }
      reconnect();}
    else {
      // Client connected
      client.loop();
    }
    unsigned long now = millis();
    if (now - lastMsg > 2000) {
      lastMsg = now;
      ++value;
      snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("outTopic", msg);
    }
    
    delay(100);
}





void readHC12() {
  // ==== Storing the incoming data into a String variable
    while (HC12.available()) {             // If HC-12 has data
      incomingByte = HC12.read();          // Store each icoming byte from HC-12
      HC12readBuffer += char(incomingByte); }   // Add each byte to HC12readBuffer string variable
    delay(100);
  // ==== Sending data from one HC-12 to another via the Serial Monitor
    while (Serial.available()) {
      HC12.write(Serial.read());}
    if (HC12readBuffer.indexOf("SolarPanelPump") > 0) {
      HC12readBuffer.replace("SolarPanelPump ", "");
      solarPanelPump = HC12readBuffer.toInt();}
    else if (HC12readBuffer.indexOf("FilterPump") > 0) {
      HC12readBuffer.replace("FilterPump ", "");
      filterPump = HC12readBuffer.toInt();}
    else if (HC12readBuffer.indexOf("temperatureIn") > 0) {
      HC12readBuffer.replace("temperatureIn ", "");
      temperatureIn = HC12readBuffer.toFloat();}
    else if (HC12readBuffer.indexOf("temperatureOut") > 0) {
      HC12readBuffer.replace("temperatureOut ", "");
      temperatureOut = HC12readBuffer.toFloat();}
    HC12readBuffer = "";                       // Clear HC12readBuffer
}


void setup_wifi() {
  delay(10); 
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);}
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}


boolean reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  return client.connected();
}


#include <SoftwareSerial.h>


//Ports
  
  int temperatureInPin = A0;
  int temperatureOutPin = A2;
  int solarPanelPumpPin = 3;
  int filterPumpPin = 2;

//Serial communication
  SoftwareSerial HC12(10, 11);         // HC-12 TX Pin, HC-12 RX Pin
  byte incomingByte;
  String HC12readBuffer = "";

//Variables
  bool automatic = 0;
  int solarPanelPump = 0;
  int filterPump = 0;
  int averageArrayLength = 20;
  float temperatureIn = 0;
  float temperatureOut = 0;
  float temperatureDeltaToTurnOn = 8;
  float temperatureDeltaToTurnOff = 1;
  float R1 = 1493;
  float R2 = 1493;
  int voltage = 5;
  String input; 


void setup() {
  //Serial
    Serial.begin(9600);                   // Open serial port to computer
  //HC12
    HC12.begin(38400);                     // Open serial port to HC12
  //Pinout
    pinMode(solarPanelPumpPin, OUTPUT);
    digitalWrite(solarPanelPumpPin, LOW);
  //Instantiate a starting average temperature close to real temperature
    temperatureIn = measurements(temperatureInPin, voltage, R2, temperatureIn, averageArrayLength);
    temperatureIn = temperatureIn*averageArrayLength;
}

void loop() {
  // Get The measurements  
    delay(500);
    temperatureOut = measurements(temperatureOutPin, voltage, R1, temperatureOut, averageArrayLength );
    temperatureIn = measurements(temperatureInPin, voltage, R2, temperatureIn, averageArrayLength);
    Serial.print("temperatureOut " );
    Serial.println(temperatureOut);
    Serial.print("temperatureIn ");
    Serial.println(temperatureIn);

  //Auto or not
    switch (automatic) {
      // out, not automatic
      case 0:
          //do nothing
        break;
      case 1: 
          if (temperatureOut > (temperatureIn + temperatureDeltaToTurnOn)) digitalWrite(solarPanelPumpPin, HIGH);
          else if (temperatureOut < (temperatureIn + temperatureDeltaToTurnOff)) digitalWrite(solarPanelPumpPin, LOW);
        break;
      }
   
  //send data to other arduino. If no HC12 present you can remove this code  
    if (HC12.available()) {
      readHC12();
    }
}




float measurements(int pinToRead, int voltage, int R1, float temperatureOldAverage, int averageLength) {
    float buffer, Vout, temperatureAverage, temperature, R2;
    int raw;
    raw = analogRead(pinToRead);
    buffer = raw * voltage;
    Vout = (buffer)/1024.0;
    buffer = (voltage/Vout) - 1;
    R2= R1 * buffer;
    temperature = 67.51 - 0.039136*R2 + 0.000007751993*R2*R2; 
    temperatureAverage = temperatureOldAverage + 1.00000/averageLength*(temperature - temperatureOldAverage);
    return temperatureAverage;}

void readHC12() {
      HC12readBuffer = "";                      // Clear HC12readBuffer 
      
  // ==== Storing the incoming data into a String variable
    while (HC12.available()) {                  // If HC-12 has data
      input = HC12.readStringUntil('\n');
      incomingByte = HC12.read();               // Store each icoming byte from HC-12
      HC12readBuffer += char(incomingByte); }   // Add each byte to HC12readBuffer string variable
    delay(100);
  // ==== Sending data from one HC-12 to another via the Serial Monitor
    while (Serial.available()) {
      HC12.write(Serial.read());}
    if (HC12readBuffer.indexOf("solarPanelPump") > 0) {
      HC12readBuffer.replace("solarpanelpump ", "");
      solarPanelPump = HC12readBuffer.toInt();}
    else if (HC12readBuffer.indexOf("filterPump") > 0) {
      HC12readBuffer.replace("filterpump ", "");
      filterPump = HC12readBuffer.toInt();}
    else if (HC12readBuffer.indexOf("temperatureIn") > 0) {
      HC12readBuffer.replace("temperaturein ", "");
      temperatureIn = HC12readBuffer.toFloat();}
    else if (HC12readBuffer.indexOf("temperatureOut") > 0) {
      HC12readBuffer.replace("temperatureout ", "");
      temperatureOut = HC12readBuffer.toFloat();}
    else if (HC12readBuffer.indexOf("automatic") > 0) {
      HC12readBuffer.replace("automatic ", "");
      automatic = HC12readBuffer.toFloat();}
    else if (HC12readBuffer.indexOf("temperatureDeltaToTurnOn") > 0) {
      HC12readBuffer.replace("temperatureDeltaToTurnOn ", "");
      temperatureDeltaToTurnOn = HC12readBuffer.toFloat();}
    else if (HC12readBuffer.indexOf("temperatureDeltaToTurnOff") > 0) {
      HC12readBuffer.replace("temperatureDeltaToTurnOff ", "");
      temperatureDeltaToTurnOff = HC12readBuffer.toFloat();}
}

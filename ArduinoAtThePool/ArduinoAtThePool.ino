
#include <SoftwareSerial.h>


//Ports
  
  int temperatureInPin = A0;
  int voltagePin = A1;
  int temperatureOutPin = A2;
  int solarPanelPumpPin = 3;
  int filterPumpPin = 2;

//Serial communication
  SoftwareSerial HC12(10, 11);         // HC-12 TX Pin, HC-12 RX Pin
  byte incomingByte;
  String HC12readBuffer = "";

//Variables
  bool automatic = 1;
  int solarPanelPump = 0;
  int filterPump = 0;
  int averageArrayLength = 20;
  float temperatureIn = 0;
  float temperatureOut = 0;
  float temperatureDeltaToTurnOn = 6;
  float temperatureDeltaToTurnOff = 2;
  float R1 = 100300;
  float R2 = 100300;
  float resistanceError = 76;
  float voltage = 5.27;
  String input; 


void setup() {
  //Led for feedback
    pinMode(LED_BUILTIN, OUTPUT);
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
    temperatureOut = measurements(temperatureOutPin, voltage, R2, temperatureOut, averageArrayLength);
    temperatureOut = temperatureOut*averageArrayLength;
}

void loop() {
  // Get The measurements  
    delay(500);
    Serial.print("voltage " );
    Serial.println(voltage);
    Serial.print("Temperature Out ");
    temperatureOut = measurements(temperatureOutPin, voltage, R1, temperatureOut, averageArrayLength );
    Serial.print("Temperature In  ");
    temperatureIn = measurements(temperatureInPin, voltage, R2, temperatureIn, averageArrayLength);


  //Auto or not
    switch (automatic) {
      // out, not automatic
      case 0:
          //do nothing
        break;
      case 1: 
          if (temperatureOut > (temperatureIn + temperatureDeltaToTurnOn)) {digitalWrite(solarPanelPumpPin, HIGH); digitalWrite(LED_BUILTIN, HIGH);}       
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
      Serial.print("      Vout ");
      Serial.print(Vout);
    buffer = (voltage/Vout) - 1;
    R2= R1 * buffer;
      Serial.print("    R2 ");
      Serial.print(R2);
    temperature = 47.51 - 0.039136*R2/resistanceError + 0.000007751993*R2/resistanceError*R2/resistanceError; 
    temperatureAverage = temperatureOldAverage + 1.00000/averageLength*(temperature - temperatureOldAverage);
      Serial.print("    temperatureAverage ");
      Serial.println(temperatureAverage);
    return temperatureAverage;}

void readHC12() {
      HC12readBuffer = "";                      // Clear HC12readBuffer 
      
  // ==== Storing the incoming data into a String variable
    while (HC12.available()) {    // If HC-12 has data
      delay(2);
      input = HC12.readStringUntil('\n');
      incomingByte = HC12.read();               // Store each icoming byte from HC-12
      HC12readBuffer += char(incomingByte); }   // Add each byte to HC12readBuffer string variable
}

void writeHC12() {
    HC12.write(automatic +","+ solarPanelPump +","+ filterPump +","+ temperatureIn +","+ temperatureOut +","+ temperatureDeltaToTurnOn +","+ temperatureDeltaToTurnOff);
    Serial.write(automatic +","+ solarPanelPump +","+ filterPump +","+ temperatureIn +","+ temperatureOut +","+ temperatureDeltaToTurnOn +","+ temperatureDeltaToTurnOff);

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

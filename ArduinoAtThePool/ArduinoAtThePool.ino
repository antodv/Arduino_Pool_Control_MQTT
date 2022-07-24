
#include <SoftwareSerial.h>


//Ports
  
  int temperatureInPin = A0;
  int temperatureOutPin = A2;
  int solarPanelPumpPin = 3;
  int filterPumpPin = 1;

//Serial communication
  SoftwareSerial HC12(1, 0);         // HC-12 RX Pin, HC-12 TX Pin
  //byte incomingByte;
  //String HC12readBuffer = "";
  //String input; 


//Variables
  int averageArrayLength = 10;
  float R1 = 100300;
  float R2 = 100300;
  float voltage = 5.27;
  int displayDelay = 500;
  unsigned long lastCurrentTime;
  
  struct dataToTransfer{
    bool automatic;                   // 1 byte
    bool solarPanelPumpStatus;        // 1 byte
    bool filterPanelPumpStatus;       // 1 byte
    int threeWayValveStatus;          // 2 byte
    int temperatureDeltaToTurnOn;     // 2
    int temperatureDeltaToTurnOff;    // 2
    float temperatureIn;              // 4
    float temperatureOut;             // 4
    };  
  const int lengthdataToTransfer = 17; //with lengthdataToTransfer the sum of the length of the datatypes in the structure
  
  union inputFromPC {
    dataToTransfer currentStatus;
    byte pcLine[lengthdataToTransfer];
  }; 
  byte pcData[lengthdataToTransfer];
  inputFromPC dataset;

void setup() {
  //Serial
    Serial.begin(115200);                   // Open serial port to computer
  //HC12
    HC12.begin(38400);                     // Open serial port to HC12
  //Pinout
    pinMode(solarPanelPumpPin, OUTPUT);
    digitalWrite(solarPanelPumpPin, HIGH);
  //Instantiate a starting average temperature close to real temperature
    dataset.currentStatus.automatic = 1;
    dataset.currentStatus.solarPanelPumpStatus = 1;
    dataset.currentStatus.filterPanelPumpStatus =0;
    dataset.currentStatus.threeWayValveStatus = 0;
    dataset.currentStatus.temperatureDeltaToTurnOn = 5;
    dataset.currentStatus.temperatureDeltaToTurnOff = 1;
    dataset.currentStatus.temperatureIn = measurements(temperatureInPin, voltage, R2, dataset.currentStatus.temperatureIn, averageArrayLength);
    dataset.currentStatus.temperatureIn = dataset.currentStatus.temperatureIn*averageArrayLength;
    dataset.currentStatus.temperatureOut = measurements(temperatureOutPin, voltage, R2, dataset.currentStatus.temperatureOut, averageArrayLength);
    dataset.currentStatus.temperatureOut = dataset.currentStatus.temperatureOut*averageArrayLength;
}

void loop() {
  // Get The measurements  
    Serial.print("temperatureOut " );
    dataset.currentStatus.temperatureOut = measurements(temperatureOutPin, voltage, R1, dataset.currentStatus.temperatureOut, averageArrayLength );
    Serial.print("temperatureIn  ");
    dataset.currentStatus.temperatureIn = measurements(temperatureInPin, voltage, R2, dataset.currentStatus.temperatureIn, averageArrayLength);
  
  //actions
    switch (dataset.currentStatus.automatic) {
      // out, not automatic
      case 0:
          //do nothing
        break;
      case 1: 
          if (dataset.currentStatus.temperatureOut > (dataset.currentStatus.temperatureIn + dataset.currentStatus.temperatureDeltaToTurnOn)) {
            digitalWrite(solarPanelPumpPin, HIGH); 
            dataset.currentStatus.solarPanelPumpStatus = 1;}
          else if (dataset.currentStatus.temperatureOut < (dataset.currentStatus.temperatureIn + dataset.currentStatus.temperatureDeltaToTurnOff)) {
            digitalWrite(solarPanelPumpPin, LOW);
            dataset.currentStatus.solarPanelPumpStatus = 0;}
        break;
      }
    //Reset value if nan
    nanReset();

  //send data to other arduino. If no HC12 present you can remove this code 
    unsigned long currentTime = millis()-8;
    Serial.println(currentTime); 
    readHC12();
    // write the data every displayDelay
      if ((currentTime - lastCurrentTime) > displayDelay)  {
        lastCurrentTime = currentTime;
        writeHC12();}
               
  delay(500);  
}




float measurements(int pinToRead, int voltage, int R1, float temperatureOldAverage, int averageLength) {
    float buffer, Vout, temperatureAverage, temperature, R2;
    int raw;
    raw = analogRead(pinToRead);
    buffer = raw * voltage;
    Vout = (buffer)/1024.0;
    buffer = (voltage/Vout) - 1;
    R2= R1 * buffer;
    temperature = 47.51 - 0.039136*R2/76 + 0.000007751993*R2/76*R2/76; 
    temperatureAverage = temperatureOldAverage + 1.00000/averageLength*(temperature - temperatureOldAverage);
    Serial.println((String)"        R2: " + R2 +(String) "        temp: " + temperature +(String)"        tempAverage: "+ temperatureAverage );
    return temperatureAverage;}

void nanReset() {
  if (isnan(dataset.currentStatus.temperatureIn) and analogRead(temperatureInPin)> 20 ) {
    dataset.currentStatus.temperatureIn = measurements(temperatureInPin, voltage, R2, 0, averageArrayLength);
    dataset.currentStatus.temperatureIn = dataset.currentStatus.temperatureIn*averageArrayLength;}
  if (isnan(dataset.currentStatus.temperatureOut) and analogRead(temperatureOutPin)> 20) {
    dataset.currentStatus.temperatureOut = measurements(temperatureOutPin, voltage, R2, 0, averageArrayLength);
    dataset.currentStatus.temperatureOut = dataset.currentStatus.temperatureOut*averageArrayLength;}   
}

void readHC12() {
   if (Serial.available() < lengthdataToTransfer) {
     return;}
     
   for (byte n = 0; n < lengthdataToTransfer; n++) {
     pcData[n] = Serial.read();}
 
   for (byte n = 0; n < 11; n++) {
     dataset.pcLine[n] = pcData[n];}

   Serial.println("data read: " + dataset.currentStatus.automatic 
    + (String)""+ dataset.currentStatus.solarPanelPumpStatus 
    + (String)""+ dataset.currentStatus.filterPanelPumpStatus
    + (String)""+ dataset.currentStatus.threeWayValveStatus 
    + (String)""+ dataset.currentStatus.temperatureDeltaToTurnOn
    + (String)""+ dataset.currentStatus.temperatureDeltaToTurnOff 
    + (String)""+ dataset.currentStatus.temperatureIn 
    + (String)""+ dataset.currentStatus.temperatureOut );
      
  /* ==== Storing the dataToTransfer data into a String variable
         HC12readBuffer = "";                      // Clear HC12readBuffer 
    while (HC12.available()) {    // If HC-12 has data
      delay(2);
      input = HC12.readStringUntil('\n');
      dataToTransferByte = HC12.read();               // Store each icoming byte from HC-12
      HC12readBuffer += char(dataToTransferByte); }   // Add each byte to HC12readBuffer string variable
  */
}

void writeHC12() { 
    HC12.write(dataset.currentStatus.automatic);
    /* 
    + dataset.currentStatus.solarPanelPumpStatus
    + dataset.currentStatus.filterPanelPumpStatus
    + dataset.currentStatus.threeWayValveStatus 
    + dataset.currentStatus.temperatureDeltaToTurnOn
    + dataset.currentStatus.temperatureDeltaToTurnOff 
    + dataset.currentStatus.temperatureIn 
    + dataset.currentStatus.temperatureOut);*/   
    Serial.print("HC12 write: ");
    Serial.println(HC12.read());
    Serial.println("data written: " + dataset.currentStatus.automatic 
    + (String)""+ dataset.currentStatus.solarPanelPumpStatus 
    + (String)""+ dataset.currentStatus.filterPanelPumpStatus
    + (String)""+ dataset.currentStatus.threeWayValveStatus 
    + (String)""+ dataset.currentStatus.temperatureDeltaToTurnOn
    + (String)""+ dataset.currentStatus.temperatureDeltaToTurnOff 
    + (String)""+ dataset.currentStatus.temperatureIn 
    + (String)""+ dataset.currentStatus.temperatureOut );
}  

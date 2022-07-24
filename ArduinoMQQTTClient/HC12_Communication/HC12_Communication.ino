#include <SoftwareSerial.h>

//Ports
  
//Serial communication
  SoftwareSerial HC12(10, 11);         // HC-12 TX Pin, HC-12 RX Pin
  byte dataToTransferByte;
  String HC12readBuffer = "";

//Variables
  unsigned long lastCurrentTime;
  String input; 
  int displayDelay = 2000;

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
    dataToTransfer dataToTransferDataTwo;
    byte pcLine[lengthdataToTransfer];
  }; 
 
//dataToTransferData

  inputFromPC dataToTransferData2;
  dataToTransfer dataToTransferData;
  byte pcData[lengthdataToTransfer];


void setup() {
  //Serial
    Serial.begin(115200);                   // Open serial port to computer
  //HC12
    HC12.begin(38400);                     // Open serial port to HC12
  //dataToTransferData
}

void loop() {
  // Get The measurements  
    unsigned long currentTime = millis()-8;
      Serial.println(currentTime);
    readHC12();
  // write the data every displayDelay
    if ((currentTime - lastCurrentTime) > displayDelay)  {
        lastCurrentTime = currentTime;
        writeHC12();
    }
    delay(500);  
}





void readHC12() {
   if (Serial.available() < lengthdataToTransfer) {
     return;}
     
   for (byte n = 0; n < lengthdataToTransfer; n++) {
      pcData[n] = Serial.read();}
 
   for (byte n = 0; n < 11; n++) {
     dataToTransferData2.pcLine[n] = pcData[n];}

   Serial.println("data read: " + dataToTransferData.automatic 
    + (String)""+ dataToTransferData.solarPanelPumpStatus 
    + (String)""+ dataToTransferData.filterPanelPumpStatus
    + (String)""+ dataToTransferData.threeWayValveStatus 
    + (String)""+ dataToTransferData.temperatureDeltaToTurnOn
    + (String)""+ dataToTransferData.temperatureDeltaToTurnOff 
    + (String)""+ dataToTransferData.temperatureIn 
    + (String)""+ dataToTransferData.temperatureOut );
      
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
    HC12.write(dataToTransferData.automatic + dataToTransferData.solarPanelPumpStatus
    + dataToTransferData.filterPanelPumpStatus
    + dataToTransferData.threeWayValveStatus 
    + dataToTransferData.temperatureDeltaToTurnOn
    + dataToTransferData.temperatureDeltaToTurnOff 
    + dataToTransferData.temperatureIn 
    + dataToTransferData.temperatureOut);   
    Serial.print("HC12 write: ");
    Serial.println( HC12.read());
    Serial.println("data write: " + dataToTransferData.automatic 
    + (String)""+ dataToTransferData.solarPanelPumpStatus 
    + (String)""+ dataToTransferData.filterPanelPumpStatus
    + (String)""+ dataToTransferData.threeWayValveStatus 
    + (String)""+ dataToTransferData.temperatureDeltaToTurnOn
    + (String)""+ dataToTransferData.temperatureDeltaToTurnOff 
    + (String)""+ dataToTransferData.temperatureIn 
    + (String)""+ dataToTransferData.temperatureOut );
}    

#include <SoftwareSerial.h>

//Ports
  
//Serial communication
  SoftwareSerial HC12(10, 11);         // HC-12 TX Pin, HC-12 RX Pin
  byte incomingByte;
  String HC12readBuffer = "";

//Variables
  unsigned long lastCurrentTime;
  String input; 
  int solarPanelPump = 0;
  int filterPump = 0;
  int displayDelay = 2000;
  struct incoming{
    bool automatic;                   // 1 byte
    bool solarPanelPumpStatus;        // 1 byte
    bool filterPanelPumpStatus;       // 1 byte
    int threeWayValveStatus;          // 2 byte
    int temperatureDeltaToTurnOn;     // 2
    int temperatureDeltaToTurnOff;    // 2
    float temperatureIn;              // 4
    float temperatureOut;             // 4
    };  
  const int lengthIncoming = 17; //with lengthIncoming the sum of the length of the datatypes in the structure
     
  union inputFromPC {
    incoming incomingDataTwo;
    byte pcLine[lengthIncoming];
  }; 
 
//IncomingData

  inputFromPC incomingData2;
  incoming incomingData;
  byte pcData[lengthIncoming];


void setup() {
  //Serial
    Serial.begin(115200);                   // Open serial port to computer
  //HC12
    HC12.begin(38400);                     // Open serial port to HC12
  //IncomingData
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
   if (Serial.available() < lengthIncoming) {
     return;}
     
   for (byte n = 0; n < lengthIncoming; n++) {
      pcData[n] = Serial.read();}
 
   for (byte n = 0; n < 11; n++) {
     incomingData2.pcLine[n] = pcData[n];}

   Serial.println("data read: " + incomingData.automatic 
    + (String)""+ incomingData.solarPanelPumpStatus 
    + (String)""+ incomingData.filterPanelPumpStatus
    + (String)""+ incomingData.threeWayValveStatus 
    + (String)""+ incomingData.temperatureDeltaToTurnOn
    + (String)""+ incomingData.temperatureDeltaToTurnOff 
    + (String)""+ incomingData.temperatureIn 
    + (String)""+ incomingData.temperatureOut );
      
  /* ==== Storing the incoming data into a String variable
         HC12readBuffer = "";                      // Clear HC12readBuffer 
    while (HC12.available()) {    // If HC-12 has data
      delay(2);
      input = HC12.readStringUntil('\n');
      incomingByte = HC12.read();               // Store each icoming byte from HC-12
      HC12readBuffer += char(incomingByte); }   // Add each byte to HC12readBuffer string variable
  */
}

void writeHC12() { 
    HC12.write(incomingData.automatic + incomingData.solarPanelPumpStatus
    + incomingData.filterPanelPumpStatus
    + incomingData.threeWayValveStatus 
    + incomingData.temperatureDeltaToTurnOn
    + incomingData.temperatureDeltaToTurnOff 
    + incomingData.temperatureIn 
    + incomingData.temperatureOut);   
    Serial.print("HC12 write: ");
    Serial.println( HC12.read());
    Serial.println("data write: " + incomingData.automatic 
    + (String)""+ incomingData.solarPanelPumpStatus 
    + (String)""+ incomingData.filterPanelPumpStatus
    + (String)""+ incomingData.threeWayValveStatus 
    + (String)""+ incomingData.temperatureDeltaToTurnOn
    + (String)""+ incomingData.temperatureDeltaToTurnOff 
    + (String)""+ incomingData.temperatureIn 
    + (String)""+ incomingData.temperatureOut );
}    

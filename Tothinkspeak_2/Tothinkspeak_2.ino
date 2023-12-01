//7 Day Sprinkler & 14 Day Air

/*
  WriteMultipleFields
  
  Description: Writes values to fields 1,2,3,4 and status in a single ThingSpeak update every 20 seconds.
  
  Hardware: ESP32 based boards
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires installation of EPS32 core. See https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md for details. 
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2020, The MathWorks, Inc.
*/
#include <Wire.h>
#include "AHT20.h"
#include "AHT20_2.h"
#include <WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

// Initialize our values
float temp1, temp2;
float humi1, humi2;

String myStatus = "";

AHT20 AHT;
AHT20_2 AHT2;


void setup() {
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  delay(30000);
  
  AHT.begin();     //SDA 21, SCL 22
  AHT2.begin();   //SDA 14, SCL 13
  
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  int ret = AHT.getSensor(&humi1, &temp1);
  int ret2 = AHT2.getSensor(&humi2, &temp2);

  if(ret)     // GET DATA OK
  {
      Serial.print("7 day humidity Sprinkler: ");
      Serial.print(humi1*100);
      Serial.print("%\t 7 day temerature Sprinkler: ");
      Serial.println(temp1);
  }
  else        // GET DATA FAIL
  {
      Serial.println("GET DATA FROM AHT20 FAIL");
  }
  if(ret2)     // GET DATA OK
  {
      Serial.print("14 day humidity Air: ");
      Serial.print(humi2*100);
      Serial.print("%\t 14 day temerature Air: ");
      Serial.println(temp2);
  }
  else        // GET DATA FAIL
  {
      Serial.println("GET DATA FROM AHT20_2 FAIL");
  }  
  
  // set the fields with the values
  ThingSpeak.setField(5, humi1*100);
  ThingSpeak.setField(6, temp1);
  ThingSpeak.setField(7, humi2*100);
  ThingSpeak.setField(8, temp2);


  // set the status
  ThingSpeak.setStatus(myStatus);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  

  
  delay(60000); // Wait 20 seconds to update the channel again
}

/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <TimeLib.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "WiFiManager.h"  
#include "Adafruit_SHT31.h"
#include <Nextion.h>
#include <NextionPage.h>
#include <NextionText.h>
#include <NextionGauge.h>
#include <SoftwareSerial.h>

//OLED ADDRESS
#define OLED_I2C_ADDRESS 0x3C


String monthSt[] ={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};


float t=0;
float h=0;



const int ESPLED = D4;

SoftwareSerial nextion(D6,D4);




//Variables for dust sensor
int DUSTSENSOR_APIN = A0; //Connect dust sensor to Arduino A0 pin
int DUSTSENSOR_LED = D3;   //Connect 3 led driver pins of dust sensor to Arduino D2

int buzzer = D0;
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
  
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;
//---------------------------

//Setup for DHT
//DHT dht(DHTPIN, DHTTYPE);
//--------------------------

//Setup for SHT31
Adafruit_SHT31 sht31 = Adafruit_SHT31();


//Setup for BMP180
SFE_BMP180 pressure;

//Setup for OLED
SSD1306AsciiWire oled;




Nextion nex(nextion);
NextionText tempt = NextionText(nex,0, 6, "tempt");
NextionText humidt = NextionText(nex,0, 9, "humidt");
NextionText pressuret = NextionText(nex,0, 5, "pressuret");
NextionText dustt = NextionText(nex,0, 11, "dustt");
NextionGauge tempg = NextionGauge(nex,0,2,"temp");
NextionGauge humidg = NextionGauge(nex,0,3,"humid");



void setup ( void ) {
  Serial.begin ( 9600 );


  
	pinMode ( ESPLED, OUTPUT );
  pinMode(DUSTSENSOR_LED,OUTPUT);

 
  digitalWrite ( ESPLED, 1 ); //turn off


  //Setup SHT31
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }


  delay(5000);
  


  //Set up for pressure sensor
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail (disconnected?)\n\n");
  }

  nextion.begin(9600);
  nex.init();

}



void loop ( void ) {

  //read DHT31
  t = sht31.readTemperature();
  h = sht31.readHumidity();




  //Read Dust Sensor
  digitalWrite(DUSTSENSOR_LED,LOW); // power on the LED
  delayMicroseconds(samplingTime);
  
  voMeasured = analogRead(DUSTSENSOR_APIN); // read the dust value
  
  delayMicroseconds(deltaTime);
  digitalWrite(DUSTSENSOR_LED,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);

  calcVoltage = voMeasured * (3.3 / 1024.0);
  dustDensity = 0.17 * calcVoltage - 0.1;

  if (dustDensity<0)
    dustDensity=0;
  Serial.print(" Dust Density: ");
  Serial.print(dustDensity * 1000);
  Serial.println(" ug/m3 ");
  //------End Read Dust------------------

  //Read Pressure
  char status;
  double T,P;
  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);
    status = pressure.getTemperature(T);
    if (status != 0)
    {
      status = pressure.startPressure(3);
      if (status != 0)
      {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P,T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P,2);
        }
      }
    }
  }
  // --- end read Pressure ---


  char hum[10];
  dtostrf(h,1,1,hum);
  humidt.setText(hum);
  int anglehumid = (int) (h * 1.8);
  humidg.setValue(anglehumid);
    
  char temp[10];
  dtostrf(t,1,1,temp);
  //tempt.setText(temp);
  char temptconcat[80];
  char tempttxt[] = "tempt.txt=\"";
  char posttempttxt[] = "\"";
  sprintf(temptconcat,"%s%s%s",tempttxt,temp,posttempttxt);
  
  tempt.setText(temp);

  int angletemp = (int) (t * 3.6);
  if(angletemp<0)
    angletemp = 360 + angletemp;

  tempg.setValue(angletemp);
  


  char dust[10];
  dtostrf(dustDensity*1000,1,2,dust);
  dustt.setText(dust);

  char pressure[10];
  dtostrf(P,1,1,pressure);
  pressuret.setText(pressure);



  delay(3000);
}





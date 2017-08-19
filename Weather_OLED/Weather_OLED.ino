#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "Adafruit_SHT31.h"
#include <SFE_BMP180.h>

//Define address for OLED library
#define I2C_ADDRESS 0x3C



SSD1306AsciiWire oled;

//Setup for SHT31
Adafruit_SHT31 sht31 = Adafruit_SHT31();
float t;
float h;

SFE_BMP180 pressure;



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



//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);   
  Wire.begin();         
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.set400kHz();  
  oled.setFont(Adafruit5x7);  

  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail (disconnected?)\n\n");
  }  
  pinMode(DUSTSENSOR_LED,OUTPUT);
}
//------------------------------------------------------------------------------
void loop() 
{
  oled.clear(); 
  oled.set2X();

  //Temperature
  t = sht31.readTemperature();
  h = sht31.readHumidity();
  char temperature[10];
  dtostrf(t,1,1,temperature);

  char hum[10];
  dtostrf(h,1,1,hum);
  //-----------------

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
  
  oled.println(String(temperature) + "c");
  oled.println(String(hum) + "%");
  oled.println(String(P) + "hpa");
  oled.println(String(dustDensity*1000) );
  delay(5000);  
}

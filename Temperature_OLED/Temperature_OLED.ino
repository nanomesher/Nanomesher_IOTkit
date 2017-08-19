#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "Adafruit_SHT31.h"

//Define address for OLED library
#define I2C_ADDRESS 0x3C



SSD1306AsciiWire oled;

//Setup for SHT31
Adafruit_SHT31 sht31 = Adafruit_SHT31();
float t;
float h;

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

}
//------------------------------------------------------------------------------
void loop() 
{
  oled.clear(); 
  oled.set1X();
  oled.println("Nanomesher");
  oled.println("Temperature&Humidity");
  oled.set2X();


  t = sht31.readTemperature();
  h = sht31.readHumidity();
  char temperature[10];
  dtostrf(t,1,1,temperature);

  char hum[10];
  dtostrf(h,1,1,hum);
  
  oled.println(String(temperature) + "c");
  oled.println(String(hum) + "%");
  
  delay(1000);  
}

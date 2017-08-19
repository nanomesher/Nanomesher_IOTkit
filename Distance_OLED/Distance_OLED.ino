#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <NewPing.h>

//Define address for OLED library
#define I2C_ADDRESS 0x3C

//Define ports for Distance Sensor
#define TRIGGER_PIN  D8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     D7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


SSD1306AsciiWire oled;
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);   
  Wire.begin();         
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.set400kHz();  
  oled.setFont(Adafruit5x7);  

}
//------------------------------------------------------------------------------
void loop() 
{
  int distance = sonar.ping_cm();
  Serial.println(distance);
  oled.clear(); 
  oled.set1X();
  oled.println("Nanomesher");
  oled.println("Distance Sensor");
  oled.set2X();
  String d = "-";

  if(distance!=0)
    d = String(distance);
  oled.println(d + " cm");
  delay(1000);  
}

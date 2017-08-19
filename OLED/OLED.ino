#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define I2C_ADDRESS 0x3C

SSD1306AsciiWire oled;
//------------------------------------------------------------------------------
void setup() {
  Wire.begin();         
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.set400kHz();  
  oled.setFont(Adafruit5x7);  

  uint32_t m = micros();
  oled.clear();  
  oled.println("Nanomesher");
  oled.println("Arduino IOT Kit");
  oled.println();
  oled.set2X();
  oled.println("BIG HELLO");
  oled.set1X();
  oled.print("small hello");
}
//------------------------------------------------------------------------------
void loop() {}

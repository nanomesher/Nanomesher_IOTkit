#include <Wire.h>

#include <NewPing.h>
#include <Nextion.h>
#include <NextionText.h>
#include <SoftwareSerial.h>


//Define address for OLED library
#define I2C_ADDRESS 0x3C

//Define ports for Distance Sensor
#define TRIGGER_PIN  D8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     D7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.


NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
SoftwareSerial nextion(D6, D4); // RX, TX
Nextion nex(nextion);

const int LEDPin = D5;
const int BuzzerPin = D0;



//------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);   
  pinMode(LEDPin, OUTPUT);
  pinMode(BuzzerPin, OUTPUT);

  nextion.begin(9600);
  nex.init();

}
//------------------------------------------------------------------------------
void loop() 
{
  int distance = sonar.ping_cm();
  


  char displayTextArray [4];
  sprintf (displayTextArray, "%03i", distance);
Serial.print(displayTextArray);

  NextionText text(nex, 0, 1, "t2");
  text.setText(displayTextArray);

  digitalWrite(LEDPin, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(200);                        // wait for a second
  digitalWrite(LEDPin, LOW);    // turn the LED off by making the voltage LOW
  delay(200);                       // wait for a second

  tone(BuzzerPin, 1000); // Send 1KHz sound signal...
  delay(200);        // ...for 1 sec
  noTone(BuzzerPin);     // Stop sound...

  delay(1000);  
}

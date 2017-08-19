#include <IFTTTMaker.h>

/*******************************************************************
 *  Trigger event at end of the setup                              *
 *                                                                 *
 *  adapted by Brian Lough                                         *
 *******************************************************************/
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <NewPing.h>
#include <IFTTTMaker.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Wire.h>

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

//Define ports for Distance Sensor
#define TRIGGER_PIN  D8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     D7  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

//------- Replace the following! ------
char ssid[] = "YOURSSID";       // your network SSID (name)
char password[] = "YOURWIFIPASSWORD";  // your network key
#define KEY "YOUR IFTTT_KEY"  // Get your key from https://ifttt.com/maker_webhooks  -> Documentation
#define EVENT_NAME "object_detected" // Name of your event name, set when you are creating the applet

//Define address for OLED library
#define I2C_ADDRESS 0x3C
int Threshold = 30; // 30cm threshold as object detect

SSD1306AsciiWire oled;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

WiFiClientSecure client;
IFTTTMaker ifttt(KEY, client);

void setup() {

  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);



   Wire.begin();         
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.set400kHz();  
  oled.setFont(Adafruit5x7);  
  delay(2000);
}
bool objectDetected = false;
int count=0;
int TOTALTHRESHOLD = 10;

void loop() 
{

  delay(1000);

  int distance = sonar.ping_cm();

  if(distance <= 30)
  {
      count = count + 1;

      if(count>TOTALTHRESHOLD)
      {
        IPAddress ip = WiFi.localIP();
        if(ifttt.triggerEvent(EVENT_NAME, ssid, ip.toString())){
          Serial.println("Successfully sent");
        } else
        {
          Serial.println("Failed!");
        }
        count = 0;
      }
  }
  else
  {
      count = 0;
      Serial.println("No sent");
  }

  oled.clear(); 
  oled.set1X();
  oled.println("Nanomesher");
  oled.println("Distance Sensor");
  oled.set2X();

  String d = "-";

  if(distance!=0)
    d = String(distance);
  oled.println(d + " cm");



  
}

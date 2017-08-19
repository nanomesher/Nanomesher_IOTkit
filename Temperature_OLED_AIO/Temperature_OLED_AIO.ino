#include <Wire.h>
#include <WiFiClient.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "Adafruit_SHT31.h"
#include "WiFiManager.h"  
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//Define address for OLED library
#define I2C_ADDRESS 0x3C

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "ayhlai"
#define AIO_KEY         "965ac4ee506a4ff887bb5ff7a2b1be36"


WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish temperaturefeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");

SSD1306AsciiWire oled;

//Setup for SHT31
Adafruit_SHT31 sht31 = Adafruit_SHT31();
float t;
float h;

//------------------------------------------------------------------------------

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void setup() {
  Serial.begin(9600);   
  Wire.begin();         
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.set400kHz();  
  oled.setFont(Adafruit5x7);  


  WiFiManager wifiManager;
  
  wifiManager.setAPCallback(configModeCallback);

  if(!wifiManager.autoConnect("Nanomesher")) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  } 

  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  oled.clear(); 
  oled.println(WiFi.localIP());
  delay(5000);  

}
//------------------------------------------------------------------------------
void loop() 
{

  MQTT_connect();


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

  temperaturefeed.publish(t);
  
  delay(5000);  
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  bool adafruitConnectionSuccess=true;
  uint8_t retries = 1;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 2 seconds...");
       mqtt.disconnect();
       delay(2000); 
       retries--;
       if (retries == 0) {
          adafruitConnectionSuccess = false;
          break;
       }
  }
  if(adafruitConnectionSuccess)
    Serial.println("MQTT Connected!");
  else
    Serial.println("MQTT cannot be connected!");
}

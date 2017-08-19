#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
int RECV_PIN = D7; //an IR detector connected to D6
IRrecv irrecv(RECV_PIN);

decode_results results;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the receiver

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (irrecv.decode(&results)) {
    serialPrintUint64(results.value, HEX);
    Serial.println();
    irrecv.resume(); // Receive the next value
      digitalWrite(LED_BUILTIN, 0);
      delay(200);
      digitalWrite(LED_BUILTIN, 1);
      delay(200);
      digitalWrite(LED_BUILTIN, 0);
  }
  

  delay(100);
}

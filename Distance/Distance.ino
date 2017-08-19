#include <NewPing.h>
#define TRIGGER_PIN D8
#define ECHO_PIN D7
#define MAX_DISTANCE 200
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 

void setup() {
  Serial.begin(9600);
}
void loop() {
  //Read distance
  unsigned int distance_cm = sonar.ping_cm();
  Serial.println(String(distance_cm) + "cm");
  delay(200);
}


const int BuzzerPin = D0;

void setup() {

    

}

void loop() {

    tone(BuzzerPin,1000); 
    delay(1000);       
    noTone(BuzzerPin);  
    delay(1000);   
}

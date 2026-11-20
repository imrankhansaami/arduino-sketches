#define numLed 6
int ledPins[numLed] = {13, 12, 11, 10, 9, 8}; 

void setup() {
  for (int i = 0; i < numLed; i++) {
    pinMode(ledPins[i], OUTPUT);  
  }
}

void loop() {
  for (int i = 0; i < numLed; i++) {
    int pin = ledPins[i];
      digitalWrite(pin, HIGH);
  
      delay(500);
  }

  for (int i = numLed; i >= 0; i--) {

      
      digitalWrite(ledPins[i], LOW);
delay(500);
    }
  }


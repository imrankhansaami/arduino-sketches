int ledPins[] = {13, 12, 11, 10, 9};  // Pins from 13 to 9

void setup() {
  for (int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);  // Set all pins as output
  }
}

void loop() {
  // Blink ODD pins first (n % 2 == 1)
  for (int i = 0; i < 5; i++) {
    int pin = ledPins[i];
    if (pin % 2 == 1) {
      digitalWrite(pin, HIGH);
      delay(500);
      digitalWrite(pin, LOW);
      delay(500);
    }
  }

  // Blink EVEN pins (n % 2 == 0)
  for (int i = 0; i < 5; i++) {
    int pin = ledPins[i];
    if (pin % 2 == 0) {
      digitalWrite(pin, HIGH);
      delay(500);
      digitalWrite(pin, LOW);
      delay(500);
    }
  }
}

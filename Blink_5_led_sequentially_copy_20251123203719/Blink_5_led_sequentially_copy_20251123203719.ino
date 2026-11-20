int r;

void setup() {
  for (r = 9; r <= 13; r++) {    
    pinMode(r, OUTPUT);
  }
}

void loop() {
  for (r = 9; r <= 13; r++) {
    digitalWrite(r, HIGH);
    delay(500);
  }

  for (r = 13; r >= 9; r--) {
    digitalWrite(r, LOW);
    delay(500);
  }
}

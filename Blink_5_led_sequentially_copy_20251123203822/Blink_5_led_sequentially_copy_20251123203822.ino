int r;

void setup() {
  for (int r = 2; r <=6; r++) {
    pinMode(r, OUTPUT);
  }
}

void loop() {
  for (int r = 2; r <=6; r++) {
    digitalWrite(r, HIGH);
    delay(250);
  }
  for (int r = 6; r >=2; r--) {
    digitalWrite(r, LOW);
    delay(250);
  }
}
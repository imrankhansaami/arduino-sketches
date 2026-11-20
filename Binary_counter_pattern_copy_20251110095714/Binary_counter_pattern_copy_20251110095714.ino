int leds[] = {2, 3, 4};

void setup() {
  for (int p:leds)
  pinMode(p, OUTPUT);
}

void loop() {
  for (int n = 0; n < 8; n++) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(leds[i], (n>>i) & 1);
      delay(500);
    }
  }
}
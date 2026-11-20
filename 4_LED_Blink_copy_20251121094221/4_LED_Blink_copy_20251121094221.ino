int leds[] = {2, 3, 4, 5};

void setup() {
 for (int pin: leds) pinMode(pin, OUTPUT);
}

void loop() {
  for (int pin:leds) digitalWrite(pin, HIGH);
  delay(500);
  for (int pin:leds) digitalWrite(pin, LOW);
  delay(500);
}
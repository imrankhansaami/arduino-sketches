int leds[] = {2, 3, 4, 5, 6, 7};

void setup() {
  for(int p:leds) pinMode(p, OUTPUT); 
}

void loop() {
  for(int i = 0; i < 6; i++) digitalWrite(leds[i], i%2 == 0);
  delay(500); 
  for(int i = 0; i < 6; i++) digitalWrite(leds[i], i%2 == 1);
  delay(500); 
}
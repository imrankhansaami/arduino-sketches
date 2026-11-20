/*
  Smart Parking Gate
  ==================
  An automatic barrier gate for a toy parking lot, on an Arduino Uno.

  How it works
    * An HC-SR04 ultrasonic sensor watches the approach lane.
    * A car (or your hand) closer than DETECT_CM -> the gate opens:
      the servo lifts the barrier, the green LED lights up and the 16x2
      LCD shows the status and a running car counter.
    * The gate stays open while the lane is occupied. Once the lane has
      been clear for HOLD_MS it closes again.
    * Anti-crush safety: if something appears in the lane while the gate
      is closing, the barrier stops and reopens with a warning beep.
    * The pushbutton is a manual override: press it to open a closed
      gate, or to close an open one straight away.

  Wiring
    HC-SR04   VCC->5V   GND->GND   TRIG->D3   ECHO->D4
    Button    D2 -> button -> GND      (internal pull-up, no resistor needed)
    Red LED   D7 -> 220R -> LED -> GND (closed / moving)
    Green LED D6 -> 220R -> LED -> GND (open)
    Buzzer    D8 (+) ... GND (-)
    Servo     signal->D9  VCC->5V  GND->GND
    LCD I2C   SDA->A4  SCL->A5  VCC->5V  GND->GND

  Libraries (Library Manager: Sketch > Include Library > Manage Libraries)
    * "LiquidCrystal I2C" by Frank de Brabander
    * "Servo" by Arduino
    Wire ships with the AVR core.

  If the LCD stays blank, run File > Examples > Wire > i2c_scanner and put
  the address it finds into LCD_ADDR below.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// ------------------------------------------------------------------- pins
const uint8_t PIN_TRIG      = 3;
const uint8_t PIN_ECHO      = 4;
const uint8_t PIN_BUTTON    = 2;
const uint8_t PIN_LED_GREEN = 6;
const uint8_t PIN_LED_RED   = 7;
const uint8_t PIN_BUZZER    = 8;
const uint8_t PIN_SERVO     = 9;

// ----------------------------------------------------------------- tuning
const uint8_t  LCD_ADDR      = 0x27;      // 0x27 or 0x3F
const bool     BUZZER_ACTIVE = false;     // true = active buzzer, false = passive
const long     DETECT_CM     = 40;        // car "at the gate" closer than this
const long     CLEAR_CM      = 70;        // lane "clear" farther than this
const unsigned long HOLD_MS  = 4000;      // stay open this long after the lane clears
const int      SERVO_CLOSED  = 0;         // barrier down (degrees)
const int      SERVO_OPEN    = 90;        // barrier up (degrees)
const int      MOVE_STEP_MS  = 15;        // delay per degree -> barrier speed

// ---------------------------------------------------------------- objects
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);
Servo gate;

enum GateState { GATE_CLOSED, GATE_OPEN };
GateState state = GATE_CLOSED;

unsigned long clearSince = 0;    // when the lane became clear (0 = busy)
unsigned long lastLcdMs  = 0;
int           carCount   = 0;

// ---------------------------------------------------------------- helpers
void lcdLine(uint8_t row, const char* text) {
  char buf[17];
  snprintf(buf, sizeof(buf), "%-16s", text);   // pad so old text disappears
  lcd.setCursor(0, row);
  lcd.print(buf);
}

void lcdInfo(long d) {
  char info[17];
  if (d < 0) snprintf(info, sizeof(info), "Cars:%2d  d:---cm", carCount % 100);
  else       snprintf(info, sizeof(info), "Cars:%2d  d:%3ldcm", carCount % 100, d);
  lcd.setCursor(0, 1);
  lcd.print(info);
}

void buzzerOn(unsigned int freq) {
  if (BUZZER_ACTIVE) digitalWrite(PIN_BUZZER, HIGH);
  else               tone(PIN_BUZZER, freq);
}

void buzzerOff() {
  if (BUZZER_ACTIVE) digitalWrite(PIN_BUZZER, LOW);
  else               noTone(PIN_BUZZER);
}

void beep(uint8_t times, unsigned int onMs, unsigned int offMs, unsigned int freq) {
  for (uint8_t i = 0; i < times; i++) {
    buzzerOn(freq);
    delay(onMs);
    buzzerOff();
    if (i + 1 < times) delay(offMs);
  }
}

long pingOnce() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(4);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  unsigned long us = pulseIn(PIN_ECHO, HIGH, 25000UL);   // 25 ms ~= 4 m
  if (us == 0) return -1;
  long cm = (long)(us / 58);
  return (cm > 400) ? -1 : cm;
}

long readDistanceCm() {           // median of up to three pings; -1 = nothing
  long s[3];
  uint8_t n = 0;
  for (uint8_t i = 0; i < 3; i++) {
    long d = pingOnce();
    if (d >= 0) s[n++] = d;
  }
  if (n == 0) return -1;
  for (uint8_t i = 1; i < n; i++) {          // insertion sort (n <= 3)
    long key = s[i];
    int j = (int)i - 1;
    while (j >= 0 && s[j] > key) { s[j + 1] = s[j]; j--; }
    s[j + 1] = key;
  }
  return s[n / 2];
}

bool buttonPressed() {            // true once per debounced press
  static bool raw = false, stable = false;
  static unsigned long t = 0;
  bool now = (digitalRead(PIN_BUTTON) == LOW);
  if (now != raw) { raw = now; t = millis(); }
  if (raw != stable && (millis() - t) >= 25) {
    stable = raw;
    if (stable) return true;
  }
  return false;
}

void openGate() {
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_LED_GREEN, HIGH);
  lcdLine(0, "WELCOME! Opening");
  beep(2, 90, 90, 1200);
  for (int a = SERVO_CLOSED; a <= SERVO_OPEN; a += 2) {
    gate.write(a);
    delay(MOVE_STEP_MS);
  }
  gate.write(SERVO_OPEN);
  lcdLine(0, "GATE OPEN");
}

// Returns true if the barrier made it down, false if the safety watch
// aborted the close (something appeared in the lane).
bool closeGate() {
  lcdLine(0, "Closing...");
  digitalWrite(PIN_LED_GREEN, LOW);
  digitalWrite(PIN_LED_RED, HIGH);
  beep(3, 80, 60, 900);

  for (int a = SERVO_OPEN; a >= SERVO_CLOSED; a -= 2) {
    gate.write(a);
    delay(MOVE_STEP_MS);

    long d = pingOnce();                          // anti-crush watch
    if (d >= 0 && d < DETECT_CM) {
      lcdLine(0, "SAFETY: reopening");
      beep(4, 60, 50, 1600);
      for (int b = a; b <= SERVO_OPEN; b += 2) {  // back up
        gate.write(b);
        delay(MOVE_STEP_MS);
      }
      gate.write(SERVO_OPEN);
      digitalWrite(PIN_LED_RED, LOW);
      digitalWrite(PIN_LED_GREEN, HIGH);
      lcdLine(0, "GATE OPEN");
      return false;
    }
  }

  gate.write(SERVO_CLOSED);
  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_LED_GREEN, LOW);
  beep(1, 150, 0, 700);
  lcdLine(0, "GATE CLOSED");
  return true;
}

// ------------------------------------------------------------------ setup
void setup() {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_LED_RED, HIGH);

  gate.attach(PIN_SERVO);
  gate.write(SERVO_CLOSED);

  lcd.init();
  lcd.backlight();
  lcdLine(0, "Smart Parking");
  lcdLine(1, "Gate  v1.0");
  beep(2, 80, 80, 1000);
  delay(900);
  lcdLine(0, "GATE CLOSED");
  lcdInfo(-1);
}

// ------------------------------------------------------------------- loop
void loop() {
  long d = readDistanceCm();

  if (millis() - lastLcdMs >= 200) {
    lastLcdMs = millis();
    lcdInfo(d);
  }

  bool detected  = (d >= 0 && d < DETECT_CM);
  bool laneClear = (d < 0 || d > CLEAR_CM);

  if (state == GATE_CLOSED) {
    if (detected || buttonPressed()) {
      carCount++;
      openGate();
      state = GATE_OPEN;
      clearSince = 0;
    }
  } else {
    if (buttonPressed()) {                  // manual close
      if (closeGate()) state = GATE_CLOSED;
      clearSince = 0;
    } else if (laneClear) {
      if (clearSince == 0) {
        clearSince = millis();
      } else if (millis() - clearSince >= HOLD_MS) {
        if (closeGate()) state = GATE_CLOSED;
        clearSince = 0;                     // restart hold after an abort too
      }
    } else {
      clearSince = 0;                       // lane busy again
    }
  }
}

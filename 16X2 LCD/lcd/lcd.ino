#include <LiquidCrystal.h>

// pins: rs, en, d4, d5, d6, d7
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  lcd.begin(16, 2);         // initialize the LCD with 16 columns and 2 rows
  lcd.setCursor(0, 0);
  lcd.print("hello");
  lcd.setCursor(0, 1);
  lcd.print("CS01");
}

void loop() {
  // Nothing required here for a static display.
  // If you want the display to blink once every few seconds, uncomment below:
  /*
  delay(2000);           // show for 2 seconds
  lcd.clear();           // hide
  delay(500);            // blank for 0.5s
  lcd.setCursor(0,0);    // restore text
  lcd.print("hello");
  lcd.setCursor(0,1);
  lcd.print("CS01");
  */
}

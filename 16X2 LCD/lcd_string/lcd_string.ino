#include<LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
lcd.begin (16, 2);
// initializes the LCD with 16 columns and 2 rows.
}

void loop()
{
lcd.clear();
lcd.setCursor(0,0);  //first a column then row
lcd.print("hello");
delay(200);
lcd.setCursor(0,1);
lcd.print("tareque Rahman");
delay(200);
lcd.clear();
delay(500);
}

# Arduino Sketches

A collection of Arduino sketches for learning digital I/O: LED patterns, binary counters, and a 16x2 LCD (HD44780, LiquidCrystal).

## LED sketches

| Sketch | What it does |
| --- | --- |
| `4_LED_Blink_copy_20251121094221` | Blinks 4 LEDs (pins 2–5) together, on/off every 500 ms. |
| `Binary_3_led_copy_20251121095940` | Counts 0–7 on 3 LEDs (pins 2–4) in binary, 1 s per step. |
| `Binary_counter_pattern_copy_20251110095714` | Same binary counter, but each bit updates 500 ms apart. |
| `Blink odd even with array_copy_20250811101442` | Blinks odd pins, then even pins, on pins 9–13. |
| `Blink_odd_even_with_array_copy_20251221102021` | Same odd/even blink as above (newer copy). |
| `Blink_5_led_sequentially_copy_20251123203719` | Lights 5 LEDs (pins 9–13) left to right, then clears right to left. |
| `Blink_5_led_sequentially_copy_20251123203822` | Same sequential effect on pins 2–6, 250 ms per step. |
| `Knigh_rider_effect_copy_20251110094528` | KITT-style scanner: a single lit LED sweeps back and forth over 6 LEDs. |
| `Odd_even_sir_copy_20251123205613` | Alternates odd/even LED groups on pins 2–7 every 500 ms. |
| `sketch_nov10a` | Lights 6 LEDs (pins 8–13) one after another, then turns them off one by one. |

## LCD sketches (`16X2 LCD/`)

| Sketch | What it does |
| --- | --- |
| `lcd/lcd.ino` | Static display: "hello" / "CS01" on a 16x2 LCD (pins RS 12, EN 11, D4–D7 5,4,3,2). |
| `lcd_string/lcd_string.ino` | Cycles text on both LCD rows ("hello" / "tareque Rahman") with clears in between. |

## libraries/

A copy of the standard `LiquidCrystal` library (with its examples), used by the LCD sketches. The Arduino IDE ships this library, so in normal use it is not required here — kept for reference/completeness.

## Building

Open any sketch folder in the Arduino IDE and upload. Wiring notes for the LCD sketches follow the standard LiquidCrystal tutorial: LCD RS→12, EN→11, D4–D7→5,4,3,2, R/W→GND, VO→10k pot wiper.

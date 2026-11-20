# Smart Parking Gate

An automatic barrier gate for a toy parking lot, on an Arduino Uno.

An HC-SR04 ultrasonic sensor watches the approach lane. When a car (or your hand) gets
close, a servo lifts the barrier, a buzzer chirps, the green LED lights and a 16x2 LCD
shows the status. When the lane has been clear for a few seconds, the gate closes again —
and if anything appears in the lane while it is closing, the anti-crush watch stops the
barrier and reopens it.

```
   approach lane                    gate
  ────────────────►   [HC-SR04]   ╲ servo arm
        car           ~30 cm        │
                    +-+-+-+-+-+-+-+-+
                    |   LCD 16x2    |   red LED  = closed / moving
                    +-+-+-+-+-+-+-+-+   green LED = open
```

## What each part does

| Part | Role |
| --- | --- |
| HC-SR04 | Measures the distance to the car in front of the gate |
| SG90 servo | The barrier arm: 0° = down (closed), 90° = up (open) |
| Buzzer | Welcome chirp on open, warning beeps on close, alarms on safety abort |
| Red / green LEDs | Gate state at a glance |
| Button | Manual override: open a closed gate, close an open one |
| 16x2 LCD (I2C) | Status line ("GATE OPEN", "Closing...") + car counter and live distance |

## Wiring

| Component | Pin on Uno | Notes |
| --- | --- | --- |
| HC-SR04 VCC / GND | 5V / GND | |
| HC-SR04 TRIG | D3 | |
| HC-SR04 ECHO | D4 | |
| Button leg 1 | D2 | other leg to GND; internal pull-up is used, no resistor |
| Red LED | D7 | D7 → 220Ω → LED → GND |
| Green LED | D6 | D6 → 220Ω → LED → GND |
| Buzzer | D8 | + to D8, − to GND |
| Servo signal | D9 | VCC to 5V, GND to GND |
| LCD SDA | A4 | |
| LCD SCL | A5 | VCC to 5V, GND to GND |

> If the Uno browns out or the servo jitters, power the servo from a separate 5V supply
> (share only GND with the Uno). A single SG90 on USB usually works fine.

## How the logic works

```
          distance < 40 cm                lane clear (no echo or > 70 cm)
CLOSED ───────────────────────► OPEN ──────────────────────────────► (4 s hold) ──► close
   ▲    or button pressed          ▲                                                   │
   │                               │   something < 40 cm while closing                 │
   └───────────────────────────────┴─────────────── safety abort ◄────────────────────┘
```

- Open trigger: distance under `DETECT_CM` or a button press.
- Stay open while anything is inside `CLEAR_CM`; the lane must be clear for `HOLD_MS`
  before the gate tries to close.
- Anti-crush: the closing loop pings the sensor every 2°, and reopens on anything
  closer than `DETECT_CM` (with an alarm).
- The counter on the LCD counts gate openings (cars served).

## Tuning

All knobs are constants at the top of `Smart_Parking_Gate.ino`:

| Constant | Default | Meaning |
| --- | --- | --- |
| `LCD_ADDR` | `0x27` | I2C address — use `0x3F` if that's what your backpack is |
| `BUZZER_ACTIVE` | `false` | `true` if you have an active buzzer (fixed tone) |
| `DETECT_CM` | `40` | "Car is here" distance |
| `CLEAR_CM` | `70` | "Lane is clear" distance (hysteresis) |
| `HOLD_MS` | `4000` | How long the gate waits before closing |
| `SERVO_CLOSED` / `SERVO_OPEN` | `0` / `90` | Barrier angles — adjust to your arm |
| `MOVE_STEP_MS` | `15` | `1` = fast/snappy, `30` = slow/heavy |

## Libraries

Install two libraries from the Library Manager (Sketch → Include Library → Manage
Libraries):

- **LiquidCrystal I2C** by Frank de Brabander
- **Servo** by Arduino

`Wire` ships with the AVR core.

## Try it

1. Upload, and watch the LCD boot to `Smart Parking / Gate v1.0`, then `GATE CLOSED`.
2. Hold your hand ~30 cm in front of the HC-SR04 while the gate is closed →
   chirp, barrier lifts, green LED, LCD says `WELCOME! Opening` then `GATE OPEN`.
3. Take your hand away → after 4 seconds the gate closes with warning beeps.
4. While it is closing, move your hand back in → `SAFETY: reopening` + alarm, and the
   gate opens again.
5. Press the button with the gate closed → opens. Press again while open → closes.

## Troubleshooting

| Symptom | Fix |
| --- | --- |
| LCD blank / only backlight | Wrong I2C address: run File → Examples → Wire → i2c_scanner, set `LCD_ADDR` to the address it prints (usually `0x27` or `0x3F`) |
| Buzzer silent | Flip `BUZZER_ACTIVE`, and check the + leg is on D8 |
| Servo twitches, board resets | Power the servo from a separate 5V source (common GND) |
| Gate opens/closes too eagerly | Raise `DETECT_CM` down / `CLEAR_CM` up, or increase `HOLD_MS` |
| Distance readings jump | Keep the sensor pointed along the lane, not at your desk clutter; the sketch already takes the median of three pings |

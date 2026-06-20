# Dual Motor Sequential Rotation Controller (Arduino & ESP32)

A tutorial project that rotates two DC motors through a fixed CW/CCW sequence while showing live status on a 20x4 I2C LCD. Two versions are included — one for classic Arduino boards, one for ESP32.

## 🎬 How it works

1. LCD powers on
2. Project name slides in from the left edge and exits off the right
3. Screen blinks for 3 seconds
4. `READY` is displayed
5. 1 second pause
6. Motor sequence runs (see below), LCD shows `MOTOR 1: <dir>` / `MOTOR 2: <dir>` each step
7. `FINISH` is displayed and motors stop

## 🔁 Sequence reference

| Step | Motor 1 | Motor 2 |
|------|---------|---------|
| 1 | CW | CW |
| 2 | CCW | CCW |
| 3 | CCW | CW |
| 4 | CW | CCW |

**Play order:** 1 → 2 → 3 → 4 → 3 → 2 → 1

### ⚠️ Motor direction is not guaranteed by wiring alone

CW/CCW in the code and on the LCD mean **clockwise/counter-clockwise as viewed from above, looking down at the wheel** (both motors are mounted shaft-up, so this reference works for either one). In the sketch, "CW" is really just `IN1=HIGH, IN2=LOW` — whether that physically spins the wheel clockwise depends on which of the motor's two wires you connected to OUT1 vs OUT2 (same idea for Motor 2 / OUT3 / OUT4). There's no universal "correct" way to wire a DC motor's two leads, so this has to be checked after wiring, not assumed from a table.

**Calibration steps:**
1. Wire both motors to the L298N however is convenient
2. Upload the sketch and watch the first sequence step (both motors should turn the same way — Step 1 is CW/CW)
3. If a motor spins the wrong direction relative to the other, swap that motor's two wires at the L298N OUT terminal — no code changes needed
4. Re-run and confirm both motors now agree with what the LCD displays

## 🧰 Components

| # | Component | Qty |
|---|-----------|-----|
| 1 | Arduino or ESP32 (MCU) | 1 |
| 2 | 18650 3.7V Li-ion battery | 2 |
| 3 | Geared DC motor 130, dual shaft, 6V (with wheels) | 2 |
| 4 | L298N motor driver | 1 |
| 5 | 20x4 I2C LCD screen | 1 |
| 6 | 5mm Sintra board (mounting plate) | 1 |

## 🔌 Pin Configuration

### Arduino

**L298N Motor Driver**

| L298N Pin | Arduino Pin | Function |
|-----------|-------------|----------|
| ENA | D9 | Motor 1 speed (PWM) |
| IN1 | D8 | Motor 1 direction |
| IN2 | D7 | Motor 1 direction |
| ENB | D10 | Motor 2 speed (PWM) |
| IN3 | D6 | Motor 2 direction |
| IN4 | D5 | Motor 2 direction |
| 12V | Battery + (2x 18650 in series) | Motor power |
| GND | Common ground | Shared with Arduino and battery |
| 5V OUT | Arduino 5V *(optional)* | Only if the onboard regulator jumper is in place |

**LCD (20x4, I2C)**

| LCD Pin | Arduino Pin |
|---------|-------------|
| SDA | A4 |
| SCL | A5 |
| VCC | 5V |
| GND | GND |

**Motors**

| Motor | Connects to |
|-------|-------------|
| Motor 1 | L298N OUT1 / OUT2 |
| Motor 2 | L298N OUT3 / OUT4 |

**Battery**

| Battery | Connects to |
|---------|-------------|
| 2x 18650 (in series, 7.4V) | L298N 12V input terminal |

### ESP32

**L298N Motor Driver**

| L298N Pin | ESP32 Pin | Function |
|-----------|-----------|----------|
| ENA | GPIO25 | Motor 1 speed (PWM) |
| IN1 | GPIO26 | Motor 1 direction |
| IN2 | GPIO27 | Motor 1 direction |
| ENB | GPIO32 | Motor 2 speed (PWM) |
| IN3 | GPIO33 | Motor 2 direction |
| IN4 | GPIO4 | Motor 2 direction |
| 12V | Battery + (2x 18650 in series) | Motor power |
| GND | Common ground | Shared with ESP32 and battery |
| 5V OUT | ESP32 5V/VIN *(optional)* | Only if the regulator can supply enough current for ESP32 + LCD |

**LCD (20x4, I2C)**

| LCD Pin | ESP32 Pin |
|---------|-----------|
| SDA | GPIO21 |
| SCL | GPIO22 |
| VCC | 5V (VIN) |
| GND | GND |

**Motors**

| Motor | Connects to |
|-------|-------------|
| Motor 1 | L298N OUT1 / OUT2 |
| Motor 2 | L298N OUT3 / OUT4 |

**Battery**

| Battery | Connects to |
|---------|-------------|
| 2x 18650 (in series, 7.4V) | L298N 12V input terminal |

See the **Troubleshooting** section near the bottom for common issues like a blank LCD or a motor spinning the wrong way.

## 📂 Choose your version

- [`sketches/arduino/`](sketches/arduino/) — for Arduino Uno (Nano shares the same I2C pins and should also work; **Mega does not** — its I2C pins are 20 (SDA) / 21 (SCL), not A4/A5, so the wiring table above doesn't apply to it)
- [`sketches/esp32/`](sketches/esp32/) — for ESP32 DevKit boards

## 🚀 Getting Started

### Arduino Uno

1. Open `sketches/arduino/arduino_motor_sequencer.ino` in Arduino IDE
2. Install the **LiquidCrystal I2C** library (by Frank de Brabander) via Library Manager
3. Wire components per the comments at the top of the sketch
4. Edit `PROJECT_NAME`, `MOTOR_SPEED`, and `STEP_DELAY_MS` to taste
5. Select **Tools → Board → Arduino Uno**, pick the right port, and upload

### ESP32

1. In Arduino IDE, go to **File → Preferences** and paste this into "Additional Boards Manager URLs":
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json`
2. Go to **Tools → Board → Boards Manager**, search "esp32", and install **esp32 by Espressif Systems**
   *(On recent Arduino IDE 2.x versions it may already appear in Boards Manager without needing the URL above — add the URL only if you don't see it.)*
3. Go to **Tools → Board** and select **ESP32 Dev Module**
4. If your board doesn't appear under **Tools → Port**, install the CP2102 or CH340 USB driver (check which chip is on your board)
5. Open `sketches/esp32/esp32_motor_sequencer.ino`
6. Install the **LiquidCrystal I2C** library (by Frank de Brabander) via Library Manager
7. Wire components per the comments at the top of the sketch
8. Edit `PROJECT_NAME`, `MOTOR_SPEED`, and `STEP_DELAY_MS` to taste
9. Upload — if it gets stuck on a string of dots ("Connecting....."), hold the **BOOT** button on the board until the upload starts

## 🔋 Battery Safety

This project uses 2x 18650 Li-ion cells in series — a few precautions worth following:

- Prefer **protected** 18650 cells (they have built-in over-discharge and short-circuit protection)
- Never let the bare terminals touch metal (tools, screws, the Sintra board's mounting hardware) — this can short the cells
- Double-check polarity before connecting to the L298N's 12V terminal; reversed polarity can damage the driver instantly
- Avoid discharging below ~3.0V per cell
- Charge only with a charger rated for Li-ion 18650 cells — never a generic or non-Li-ion charger

## 🛠 Troubleshooting

| Symptom | Likely cause | Fix |
|---|---|---|
| LCD backlight on but no text | Wrong I2C address | Try `0x3F` instead of `0x27` in `LCD_ADDR` |
| LCD shows garbled/random characters | Loose SDA/SCL wiring | Recheck I2C wiring and connections |
| Motor spins the wrong direction | Motor leads "reversed" | Swap that motor's two wires at the L298N output terminal |
| One or both motors don't spin | No motor power or bad common ground | Confirm battery is wired to the L298N's 12V terminal and all grounds are tied together |
| ESP32 upload stuck on "Connecting....." | Board not entering flash mode | Hold the **BOOT** button while the upload starts |
| "esp32" not found in Boards Manager | Missing boards manager URL | Add `https://espressif.github.io/arduino-esp32/package_esp32_index.json` under Additional Boards Manager URLs |
| ESP32 missing from Tools → Port | Missing USB driver | Install the CP2102 or CH340 driver matching your board |
| `LiquidCrystal_I2C.h` not found | Library not installed | Install "LiquidCrystal I2C" by Frank de Brabander via Library Manager |

## 📜 License

MIT — see [LICENSE](LICENSE)

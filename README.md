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

> If the LCD stays blank or shows garbled text, try I2C address `0x3F` instead of `0x27` (change `LCD_ADDR` in the sketch). If a motor spins the wrong way, swap its two wires at the L298N terminal instead of editing code.

## 📂 Choose your version

- [`sketches/arduino/`](sketches/arduino/) — for Arduino Uno/Nano/Mega
- [`sketches/esp32/`](sketches/esp32/) — for ESP32 DevKit boards

Each sketch also repeats its wiring table in the header comment, so it's handy while you're wiring at the bench.

## 🚀 Getting Started

1. Pick your board and open the matching `.ino` file
2. Install the **LiquidCrystal I2C** library (by Frank de Brabander) via Library Manager
3. Wire components per the comments at the top of the sketch
4. Edit `PROJECT_NAME`, `MOTOR_SPEED`, and `STEP_DELAY_MS` to taste
5. Upload and power on

## 📜 License

MIT
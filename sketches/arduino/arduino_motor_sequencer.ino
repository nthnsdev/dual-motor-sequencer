/*
  Dual Motor Sequential Rotation Controller — ARDUINO VERSION
  -------------------------------------------------------------
  Boot sequence:
    1. LCD turns on
    2. Project name slides in from the left and exits off the right
    3. Screen blinks for 3 seconds
    4. "READY" is displayed
    5. 1 second delay
    6. Motor sequence runs: 1 -> 2 -> 3 -> 4 -> 3 -> 2 -> 1
    7. "FINISH" is displayed, motors stop

  Hardware:
    - Arduino Uno (or compatible)
    - L298N motor driver
    - 2x Geared DC motor 130, dual shaft, 6V
    - 20x4 I2C LCD
    - 2x 18650 3.7V battery (motor power -> L298N's 12V input,
      NOT through the Arduino's 5V pin)

  Library required (Library Manager):
    "LiquidCrystal I2C" by Frank de Brabander

  Wiring (default pins below — change the #defines if yours differ):

    L298N -> Arduino
      ENA -> D9   (PWM, Motor 1 speed)
      IN1 -> D8
      IN2 -> D7
      ENB -> D10  (PWM, Motor 2 speed)
      IN3 -> D6
      IN4 -> D5

    LCD I2C -> Arduino
      SDA -> A4
      SCL -> A5
      VCC -> 5V
      GND -> GND

    Power:
      2x 18650 in series (7.4V) -> L298N "12V" input terminal
      L298N 5V OUT -> Arduino 5V pin (only if the L298N's onboard
        5V regulator jumper is in place — remove the jumper and
        power the Arduino separately if your motors draw a lot of current)
      Battery GND, L298N GND, and Arduino GND must all be tied together

  Notes:
    - If the LCD shows nothing or garbled characters, your I2C address
      might be 0x3F instead of 0x27 — change LCD_ADDR below.
    - If a motor spins the "wrong" way, swap that motor's two wires
      at the L298N terminal instead of editing code.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------- USER SETTINGS ----------
#define LCD_ADDR        0x27   // try 0x3F if the screen stays blank
#define LCD_COLS        20
#define LCD_ROWS        4

const char* PROJECT_NAME = "YOUR PROJECT NAME";  // <-- change this

const int MOTOR_SPEED    = 200;   // 0-255
const int STEP_DELAY_MS  = 2000;  // how long each step runs
const int SLIDE_DELAY_MS = 120;   // speed of the name slide animation
// ------------------------------------

// Motor driver pins
const int ENA = 9, IN1 = 8, IN2 = 7;
const int ENB = 10, IN3 = 6, IN4 = 5;

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

enum Direction { CW, CCW };

struct Step {
  Direction m1;
  Direction m2;
};

// Steps 1-4 from the wiring/sequence diagram
Step steps[4] = {
  { CW,  CW  },  // Step 1
  { CCW, CCW },  // Step 2
  { CCW, CW  },  // Step 3
  { CW,  CCW }   // Step 4
};

// Play order: 1, 2, 3, 4, 3, 2, 1  (as indices into steps[])
int sequenceOrder[] = { 0, 1, 2, 3, 2, 1, 0 };
const int SEQUENCE_LEN = 7;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  motorsStop();

  lcd.init();
  lcd.backlight();

  bootSequence();
  runMotorSequence();
  showFinish();
}

void loop() {
  // Everything runs once in setup(). Nothing repeats here.
}

// ---------- BOOT SEQUENCE ----------
void bootSequence() {
  slideTextRight(PROJECT_NAME, 1);
  clearRow(1);

  blinkScreen(3000, 500);  // blink for 3 seconds

  lcd.setCursor(0, 1);
  lcd.print("READY");
  delay(1000);
  lcd.clear();
}

void slideTextRight(const char* text, uint8_t row) {
  int len = strlen(text);
  for (int startCol = -len; startCol <= LCD_COLS; startCol++) {
    clearRow(row);
    for (int i = 0; i < len; i++) {
      int col = startCol + i;
      if (col >= 0 && col < LCD_COLS) {
        lcd.setCursor(col, row);
        lcd.print(text[i]);
      }
    }
    delay(SLIDE_DELAY_MS);
  }
}

void clearRow(uint8_t row) {
  lcd.setCursor(0, row);
  for (int i = 0; i < LCD_COLS; i++) lcd.print(' ');
}

void blinkScreen(int totalMs, int intervalMs) {
  int cycles = totalMs / (intervalMs * 2);
  for (int i = 0; i < cycles; i++) {
    lcd.noBacklight();
    delay(intervalMs);
    lcd.backlight();
    delay(intervalMs);
  }
}

// ---------- MOTOR SEQUENCE ----------
void runMotorSequence() {
  for (int i = 0; i < SEQUENCE_LEN; i++) {
    Step s = steps[sequenceOrder[i]];
    setMotor1(s.m1);
    setMotor2(s.m2);
    displayStep(s, i + 1);
    delay(STEP_DELAY_MS);
  }
  motorsStop();
}

void displayStep(Step s, int stepNum) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Step ");
  lcd.print(stepNum);
  lcd.print("/7");

  lcd.setCursor(0, 1);
  lcd.print("MOTOR 1: ");
  lcd.print(s.m1 == CW ? "CW" : "CCW");

  lcd.setCursor(0, 2);
  lcd.print("MOTOR 2: ");
  lcd.print(s.m2 == CW ? "CW" : "CCW");
}

void setMotor1(Direction d) {
  digitalWrite(IN1, d == CW ? HIGH : LOW);
  digitalWrite(IN2, d == CW ? LOW : HIGH);
  analogWrite(ENA, MOTOR_SPEED);
}

void setMotor2(Direction d) {
  digitalWrite(IN3, d == CW ? HIGH : LOW);
  digitalWrite(IN4, d == CW ? LOW : HIGH);
  analogWrite(ENB, MOTOR_SPEED);
}

void motorsStop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ---------- FINISH ----------
void showFinish() {
  lcd.clear();
  lcd.setCursor(6, 1);
  lcd.print("FINISH");
  // Motors are already stopped here. Add a buzzer beep, an auto-restart,
  // or anything else you want to happen at the end — this is the spot for it.
}

/*
  Dual Motor Sequential Rotation Controller — ESP32 VERSION
  -------------------------------------------------------------
  Same behavior as the Arduino version — see that sketch's header
  for the full step-by-step explanation of the sequence.

  Hardware:
    - ESP32 DevKit (any variant with the pins below broken out)
    - L298N motor driver
    - 2x Geared DC motor 130, dual shaft, 6V
    - 20x4 I2C LCD
    - 2x 18650 3.7V battery (motor power -> L298N's 12V input,
      NOT through the ESP32's 5V/3V3 pin)

  Library required (Library Manager):
    "LiquidCrystal I2C" by Frank de Brabander

  Wiring (default pins below — change the consts if yours differ):

    L298N -> ESP32
      ENA -> GPIO25  (PWM, Motor 1 speed)
      IN1 -> GPIO26
      IN2 -> GPIO27
      ENB -> GPIO32  (PWM, Motor 2 speed)
      IN3 -> GPIO33
      IN4 -> GPIO4

    LCD I2C -> ESP32
      SDA -> GPIO21
      SCL -> GPIO22
      VCC -> 5V (VIN)
      GND -> GND

    Power:
      2x 18650 in series (7.4V) -> L298N "12V" input terminal
      ESP32 powered via USB, or from L298N 5V OUT if its onboard
        regulator can supply enough current for both the ESP32 and LCD
      Battery GND, L298N GND, and ESP32 GND must all be tied together

  Notes:
    - If the LCD shows nothing or garbled characters, your I2C address
      might be 0x3F instead of 0x27 — change LCD_ADDR below.
    - If a motor spins the "wrong" way, swap that motor's two wires
      at the L298N terminal instead of editing code.
    - This sketch uses the LEDC API with channels (ledcSetup +
      ledcAttachPin), which works on all ESP32 Arduino core versions.
      If you're on core 3.x and prefer the newer pin-based API, replace
      the PWM setup in setup() and the ledcWrite() calls as shown in
      the comments below.
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

#define SDA_PIN 21
#define SCL_PIN 22
// ------------------------------------

// Motor driver pins
const int ENA = 25, IN1 = 26, IN2 = 27;
const int ENB = 32, IN3 = 33, IN4 = 4;

// LEDC PWM setup
const int PWM_CH_A  = 0;
const int PWM_CH_B  = 1;
const int PWM_FREQ  = 1000;
const int PWM_RES   = 8;   // 8-bit -> duty range 0-255 (matches MOTOR_SPEED)

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
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // --- PWM setup (classic channel-based LEDC API) ---
  ledcSetup(PWM_CH_A, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENA, PWM_CH_A);
  ledcSetup(PWM_CH_B, PWM_FREQ, PWM_RES);
  ledcAttachPin(ENB, PWM_CH_B);
  // --- Core 3.x alternative (pin-based API), use instead of the 4 lines above:
  // ledcAttach(ENA, PWM_FREQ, PWM_RES);
  // ledcAttach(ENB, PWM_FREQ, PWM_RES);
  // ...and then call ledcWrite(ENA, duty) / ledcWrite(ENB, duty) instead of
  // ledcWrite(PWM_CH_A, duty) / ledcWrite(PWM_CH_B, duty) everywhere below.

  motorsStop();

  Wire.begin(SDA_PIN, SCL_PIN);
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
  ledcWrite(PWM_CH_A, MOTOR_SPEED);
}

void setMotor2(Direction d) {
  digitalWrite(IN3, d == CW ? HIGH : LOW);
  digitalWrite(IN4, d == CW ? LOW : HIGH);
  ledcWrite(PWM_CH_B, MOTOR_SPEED);
}

void motorsStop() {
  ledcWrite(PWM_CH_A, 0);
  ledcWrite(PWM_CH_B, 0);
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

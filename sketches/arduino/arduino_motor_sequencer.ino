#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR        0x27 
#define LCD_COLS        20
#define LCD_ROWS        4

const char* PROJECT_NAME = "YOURNAME"; 

const int MOTOR_SPEED    = 200; 
const int STEP_DELAY_MS  = 5000; 
const int SLIDE_DELAY_MS = 120;  

const int ENA = 9, IN1 = 8, IN2 = 7;
const int ENB = 10, IN3 = 6, IN4 = 5;

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

enum Direction { CW, CCW };

struct Step {
  Direction m1;
  Direction m2;
};

Step steps[4] = {
  { CW,  CW  }, 
  { CCW, CCW }, 
  { CCW, CW  },  
  { CW,  CCW }   
};

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
}

void bootSequence() {
  slideTextRight(PROJECT_NAME, 1);
  clearRow(1);

  blinkScreen(3000, 500);  

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

void showFinish() {
  lcd.clear();
  lcd.setCursor(6, 1);
  lcd.print("FINISH");
}

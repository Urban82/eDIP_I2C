#include <Wire.h>
#include <eDIP_I2C.h>

#define LCD_ADDRESS 0x6F

eDIP_I2C LCD(LCD_ADDRESS);

void setup() {
  Serial.begin(9600);
  LCD.begin();
}

void loop() {
  uint8_t r, f;

  LCD.displayHide();
  LCD.terminalShow();
  LCD.terminalClear();

  LCD.terminalPrintRevision();

  LCD.sendData(18 , "\n\rThis is a test\n\r");

  LCD.terminalPosition(10, 6);
  LCD.sendData(4, "Test");

  delay(100000);
}

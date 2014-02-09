#include <Wire.h>
#include <eDIP_I2C.h>

#define LCD_ADDRESS 0x6F

eDIP_I2C LCD(LCD_ADDRESS);

void setup() {
  Serial.begin(9600);
  LCD.begin();
}

void loop() {
  LCD.terminalHide();
  LCD.displayShow();

  LCD.setDrawWidth(1, 1);
  LCD.setDrawMode(EDIP_I2C_MODE_SET);
  
  LCD.displayFill();
  
  delay(2000);
  
  LCD.displayClear();
  
  delay(2000);
  
  LCD.drawPoint(5, 5);
  
  delay(1000);
  
  LCD.drawLine(10, 10, 12, 20);
  
  delay(1000);
  
  LCD.drawContinueLine(20, 30);
  
  delay(1000);
  
  LCD.drawRectangle(100, 10, 120, 20);
  
  delay(1000);
  
  LCD.clearArea(110, 5, 130, 15);
  
  delay(1000);
  
  LCD.fillArea(110, 15, 130, 25);
  
  delay(1000);
  
  LCD.invertArea(95, 4, 120, 26);
  
  delay(1000);
  
  LCD.fillArea(200, 10, 230, 40, 7);
  
  delay(1000);
  
  LCD.fillRectangle(200, 50, 230, 80, 7);
  
  delay(1000);
  
  LCD.fillArea(5, 45, 105, 85, 4);
  
  delay(1000);
  
  LCD.drawBox(10, 50, 50, 80, 9);
  
  delay(1000);
  
  LCD.fillBox(60, 50, 100, 80, 9);
  
  delay(1000);
  
  LCD.drawLine(10, 110, 20, 110);
  for (uint8_t w = 2; w < 15; ++w) {
    LCD.setDrawWidth(w, w);
    LCD.drawContinueLine(10 + w * 10, 110);
  }
  
  delay(1000);

  for (uint8_t i = 0; i < 5; ++i) {
    LCD.displayInvert();
    
    delay(2000);
  }

  delay(100000);
}

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

  LCD.setTextZoom(1, 1);
  LCD.setTextDirection(EDIP_I2C_DIRECTION_LEFT);
  LCD.setTextMode(EDIP_I2C_MODE_REPLACE);
  LCD.setTextBlink(EDIP_I2C_BLINK_OFF);
  
  LCD.displayFill();
  
  delay(2000);
  
  LCD.displayClear();
  
  delay(2000);
  
  LCD.setTextFont(EDIP_I2C_FONT_GENEVA10);
  LCD.displayString(EDIP_I2C_LEFT, 120, 0, "L~ef~t");
  
  delay(1000);
  
  LCD.setTextFont(EDIP_I2C_FONT_CHICAGO14);
  LCD.displayString(EDIP_I2C_CENTER, 120, 10, "Center");
  
  delay(1000);
  
  LCD.setTextFont(EDIP_I2C_FONT_SWISS30);
  LCD.displayString(EDIP_I2C_RIGHT, 120, 30, "R@igh@t");
  
  delay(1000);
  
  LCD.setTextFont(EDIP_I2C_FONT_MONO_8_8);
  LCD.displayString(EDIP_I2C_LEFT, 120, 50, "Terminal");
  
  delay(1000);
  
  LCD.setTextFont(EDIP_I2C_FONT_BIG_NUM);
  LCD.displayString(EDIP_I2C_CENTER, 120, 65, "123.45");
  
  delay(1000);
  
  LCD.setTextFont(EDIP_I2C_FONT_GENEVA10);
  LCD.setTextDirection(EDIP_I2C_DIRECTION_UP);
  LCD.displayString(EDIP_I2C_LEFT, 5, 122, "This go up!");
  LCD.setTextDirection(EDIP_I2C_DIRECTION_LEFT);
  
  delay(1000);

  LCD.setTextZoom(3, 3);
  LCD.displayString(EDIP_I2C_LEFT, 5, 5, "Big!");
  LCD.setTextZoom(1, 1);
  
  delay(1000);
  
  LCD.setTextBlink(EDIP_I2C_BLINK_ON);
  LCD.displayString(EDIP_I2C_RIGHT, 235, 5, "Blin~k~");
  LCD.setTextBlink(EDIP_I2C_BLINK_INVERT);
  LCD.displayString(EDIP_I2C_RIGHT, 235, 20, "Inver@t@");
  LCD.setTextBlink(EDIP_I2C_BLINK_OFF);
  
  delay(1000);
  
  for (uint8_t i = 0; i < 5; ++i) {
    LCD.displayInvert();
    
    delay(2000);
  }

  delay(100000);
}

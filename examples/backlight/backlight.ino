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
  LCD.setTextFont(EDIP_I2C_FONT_CHICAGO14);
  
  LCD.displayClear();
  
  delay(2000);
  
  LCD.backlightOff();
  LCD.displayString(EDIP_I2C_CENTER, 120, 30, "Off");
  
  delay(2000);
  
  LCD.backlightOn();
  LCD.clearArea(110, 29, 130, 45);
  LCD.displayString(EDIP_I2C_CENTER, 120, 30, "On");
  
  delay(2000);
  
  for (uint8_t i = 0; i < 100; i += 10) {
    char buffer[20];
    
    LCD.backlight(i);

    sprintf(buffer, "Lvl: %u", i);
    LCD.clearArea(90, 29, 150, 45);
    LCD.displayString(EDIP_I2C_CENTER, 120, 30, buffer);
    
    delay(1000);
  }

  delay(100000);
}

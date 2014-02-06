#include <Wire.h>
#include <eDIP_I2C.h>

#define LCD_ADDRESS 0x6F
#define ESC         0x1B

uint8_t y = 0;

eDIP_I2C LCD(LCD_ADDRESS);

void setup() {
  Serial.begin(9600);
  LCD.begin();
}

void loop() {
  if (y == 0) {
    char cmd[3] = {ESC, 'D', 'L'};
    LCD.sendData(3, cmd);
  }
  char cmd[7] = {ESC, 'G', 'D', 0, 0 + y, 239, 127 - y};
  LCD.sendData(7, cmd);

  y += 4;
  if (y == 127) {
    y = 0;
    delay(10000);
  }
  if (y > 127)
    y = 127;

  delay(250);
}

#include <Wire.h>
#include <eDIP_I2C.h>

#define LCD_ADDRESS 0x6F

eDIP_I2C LCD(LCD_ADDRESS);

char buffer[20];
int buffer_len = 0;
unsigned i = 0, pressed;

void setup() {
  Serial.begin(9600);
  LCD.begin();

  LCD.terminalHide();
  LCD.displayShow();

  LCD.displayClear();
  
  LCD.setTextFont(EDIP_I2C_FONT_CHICAGO14);
  
  LCD.displayString(EDIP_I2C_LEFT, 130, 10, "Pressed: Y");
  LCD.displayString(EDIP_I2C_LEFT, 130, 30, "Click: 0");
  
  LCD.setTouchFrame(7);
  LCD.setTouchFont(EDIP_I2C_FONT_GENEVA10);
  LCD.setTouchZoom(1, 1);
  LCD.setMenuFont(EDIP_I2C_FONT_CHICAGO14);
  LCD.setMenuZoom(1, 1);
  LCD.setTouchResponse(1);
  LCD.deleteTouchArea(0, 1);
  
  LCD.createTouchKey(10, 10, 50, 50, 'C', 0, EDIP_I2C_CENTER, "Click|here");
  LCD.createTouchSwitch(60, 10, 110, 50, 'N', 'N', EDIP_I2C_CENTER, "No click|switch");
  LCD.createTouchMenu(10, 70, 110, 100, 0, 0, 'n', EDIP_I2C_RIGHT, EDIP_I2C_CENTER, "Menu", "Reset|5|10");
  
  LCD.setTouchSwitch('N', 1);
  pressed = 1;
  
  Serial.println("Ready");
}

void loop() {
  uint8_t bytes_ready;
  
  LCD.getBufferInfo(&bytes_ready, 0);
  if (bytes_ready > 0) {
    buffer_len = LCD.getBuffer(20, buffer);
    Serial.print("Buffer size: ");
    Serial.println(buffer_len);
    if (buffer_len > 0) {
      int buffer_pos = 0;
      while (buffer_pos < buffer_len) {
        Serial.print("Buffer:");
        for (int k = 0; k < 4; ++k) {
          Serial.print(" ");
          Serial.print(buffer[buffer_pos + k], HEX);
        }
        Serial.println("");
        if (buffer[buffer_pos]     == 0x1B &&
            buffer[buffer_pos + 1] == 'A'  &&
            buffer[buffer_pos + 2] == 1) {
          // A key was pressed
          switch (buffer[buffer_pos + 3]) {
            case 'C':
              ++i;
              break;
            case 'N':
              if (pressed)
                pressed = 0;
              else
                pressed = 1;
              break;
            default:
              Serial.println("Unknown key");
              break;
          }
        } else if(buffer[buffer_pos]     == 0x1B &&
                  buffer[buffer_pos + 1] == 'N'  &&
                  buffer[buffer_pos + 2] == 1) {
          // A menu was pressed
          switch (buffer[buffer_pos + 3]) {
            case 'n':
              i = 0;
              break;
            case 'n' + 1:
              i += 5;
              break;
            case 'n' + 2:
              i += 10;
              break;
            default:
              Serial.println("Unknown menu");
              break;
          }
        }
        buffer_pos += 4;
      }
   
      sprintf(buffer, "Click: %u", i);
      LCD.clearArea(129, 29, 229, 45);
      LCD.displayString(EDIP_I2C_LEFT, 130, 30, buffer);
    }
   
    sprintf(buffer, "Pressed: %c", pressed ? 'Y' : 'N');
    LCD.clearArea(129, 9, 229, 25);
    LCD.displayString(EDIP_I2C_LEFT, 130, 10, buffer);
  }
    
  delay(500);
}

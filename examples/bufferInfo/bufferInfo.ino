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
  
  if (!LCD.getBufferInfo(&r, &f)) {
    Serial.println("Error");
  }
  else {
    Serial.println("Buffer info");
    Serial.print("Bytes ready: "); Serial.println(r);
    Serial.print("Bytes free:  "); Serial.println(f);
  }
  
  delay(100000);
}

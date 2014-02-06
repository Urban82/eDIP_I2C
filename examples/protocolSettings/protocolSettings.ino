#include <Wire.h>
#include <eDIP_I2C.h>

#define LCD_ADDRESS 0x6F

eDIP_I2C LCD(LCD_ADDRESS);

void setup() {
  Serial.begin(9600);
  LCD.begin();
}

void loop() {
  uint8_t mps, ps, to;

  if (!LCD.getProtocolSettings(&mps, &ps, &to)) {
    Serial.println("Error");
  }
  else {
    Serial.print("Max packet size: "); Serial.println(mps);
    Serial.print("Packet size:     "); Serial.println(ps);
    Serial.print("Timeout:         "); Serial.println(to);
  }

  delay(100000);
}

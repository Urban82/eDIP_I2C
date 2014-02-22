//
// FILE: eDIP_I2C.h
// VERSION: 1.0
// AUTHOR: Danilo Treffiletti
// PURPOSE: EA eDIP library for small protocol over I2C bus
//

#ifndef EDIP_I2C_H
#define EDIP_I2C_H

#if ARDUINO < 100
 #include <WProgram.h>
#else
 #include <Arduino.h>
#endif

#define EDIP_I2C_VERSION "1.0"

#define EDIP_I2C_LEFT   0x01
#define EDIP_I2C_CENTER 0x02
#define EDIP_I2C_RIGHT  0x03

#define EDIP_I2C_FONT_MONO_8_8  0x00
#define EDIP_I2C_FONT_MONO_4_6  0x01
#define EDIP_I2C_FONT_MONO_6_8  0x02
#define EDIP_I2C_FONT_MONO_7_12 0x03
#define EDIP_I2C_FONT_GENEVA10  0x04
#define EDIP_I2C_FONT_CHICAGO14 0x05
#define EDIP_I2C_FONT_SWISS30   0x06
#define EDIP_I2C_FONT_BIG_NUM   0x07

#define EDIP_I2C_DIRECTION_LEFT 0x00
#define EDIP_I2C_DIRECTION_UP   0x01

#define EDIP_I2C_MODE_SET         0x01
#define EDIP_I2C_MODE_DELETE      0x02
#define EDIP_I2C_MODE_INVERSE     0x03
#define EDIP_I2C_MODE_REPLACE     0x04
#define EDIP_I2C_MODE_INV_REPLACE 0x05

#define EDIP_I2C_BLINK_OFF    0x00
#define EDIP_I2C_BLINK_ON     0x01
#define EDIP_I2C_BLINK_INVERT 0x02

class eDIP_I2C {
public:
  eDIP_I2C(uint8_t address);

  void begin();

  bool getProtocolSettings(uint8_t *maxPacketSize, uint8_t *actualPacketSize, uint8_t *actualTimeout);

  // Buffer functions
  bool getBufferInfo(uint8_t *ready, uint8_t *free);
  int  getBuffer(const uint8_t len, char *buf);

  // RAW data function
  bool sendData(const uint8_t len, const char* buf);

  // Terminal functions
  bool terminalShow();
  bool terminalHide();
  bool terminalClear();
  bool terminalCursor(bool on);
  bool terminalPosition(uint8_t column, uint8_t line);
  bool terminalPrintRevision();

  // Display functions
  bool displayShow();
  bool displayHide();
  bool displayClear();
  bool displayFill();
  bool displayInvert();

  // Text functions
  bool displayString(uint8_t align, uint8_t x, uint8_t y, const char* text);
  bool setTextFont(uint8_t font);
  bool setTextZoom(uint8_t x, uint8_t y);
  bool setTextDirection(uint8_t direction);
  bool setTextMode(uint8_t mode);
  bool setTextBlink(uint8_t blink);

  // Draw functions
  bool drawPoint(uint8_t x, uint8_t y);
  bool drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
  bool drawContinueLine(uint8_t x, uint8_t y);
  bool drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
  bool clearArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
  bool invertArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
  bool fillArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
  bool fillArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t pattern);
  bool fillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t pattern);
  bool drawBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t type);
  bool fillBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t type);
  bool setDrawWidth(uint8_t x, uint8_t y);
  bool setDrawMode(uint8_t mode);

  // Backlight functions
  bool backlightOn();
  bool backlightOff();
  bool backlight(uint8_t level);

private:
  bool i2csend(const uint8_t code, const uint8_t len, const char* buf);
  int i2crecv(const uint8_t code, const uint8_t len, char* buf);

  uint8_t m_address;
  unsigned long m_timeout;
  uint8_t m_packetSize;
};

#endif /* EDIP_I2C_H */

//
// FILE: eDIP_I2C.cpp
// VERSION: 1.0
// AUTHOR: Danilo Treffiletti
// PURPOSE: EA eDIP library for small protocol over I2C bus
//

#include "eDIP_I2C.h"

#include <Wire.h>

#define ACK 0x06
#define NAK 0x15

#define DC1 0x11
#define DC2 0x12
#define ESC 0x1B

#define TIMEOUT      2560000
#define PACKET_SIZE  64

#define EDIP_I2C_DEBUG

static void i2cwrite(uint8_t x);
static bool i2cread(uint8_t *x, unsigned long timeout);
static void SerialPrintValue(uint8_t x);

eDIP_I2C::eDIP_I2C(uint8_t address)
  : m_address(address)
  , m_timeout(TIMEOUT)
  , m_packetSize(PACKET_SIZE)
{}

void eDIP_I2C::begin()
{
  Wire.begin();

  uint8_t ps, to;
  if (getProtocolSettings(0, &ps, &to)) {
    m_timeout = to;
    m_packetSize = ps;
  }
}

bool eDIP_I2C::getProtocolSettings(uint8_t *maxPacketSize, uint8_t *actualPacketSize, uint8_t *actualTimeout) {
  char cmd[1] = { 'P' };

  if (!i2csend(DC2, 1, cmd))
    return false;

  char settings[3];
  if (i2crecv(DC2, 3, settings) != 3)
    return false;

  if (maxPacketSize)
    *maxPacketSize = settings[0];
  if (actualPacketSize)
    *actualPacketSize = settings[1];
  if (actualTimeout)
    *actualTimeout = settings[2];

  return true;
}

// Buffer functions

bool eDIP_I2C::getBufferInfo(uint8_t *ready, uint8_t *free) {
  char cmd[1] = { 'I' };

  if (!i2csend(DC2, 1, cmd))
    return false;

  char info[2];
  if (i2crecv(DC2, 2, info) != 2)
    return false;

  if (ready)
    *ready = info[0];
  if (free)
    *free = info[1];

  return true;
}

int eDIP_I2C::getBuffer(const uint8_t len, char *buf) {
  char cmd[1] = { 'S' };

  if (!i2csend(DC2, 1, cmd))
    return -1;

  return i2crecv(DC1, len, buf);
}

// RAW data function

bool eDIP_I2C::sendData(const uint8_t len, const char* buf) {
  return i2csend(DC1, len, buf);
}

// Terminal functions

bool eDIP_I2C::terminalShow() {
  char cmd[3] = { ESC, 'T', 'E' };
  return sendData(3, cmd);
}

bool eDIP_I2C::terminalHide() {
  char cmd[3] = { ESC, 'T', 'A' };
  return sendData(3, cmd);
}

bool eDIP_I2C::terminalClear() {
  char cmd[1] = { 12 };
  return sendData(1, cmd);
}

bool eDIP_I2C::terminalCursor(bool on) {
  char cmd[4] = { ESC, 'T', 'C', on ? 1 : 0 };
  return sendData(4, cmd);
}

bool eDIP_I2C::terminalPosition(uint8_t column, uint8_t line) {
  char cmd[5] = { ESC, 'T', 'P', column, line };
  return sendData(5, cmd);
}

bool eDIP_I2C::terminalPrintRevision() {
  char cmd[3] = { ESC, 'T', 'V' };
  return sendData(3, cmd);
}

// Display functions

bool eDIP_I2C::displayShow() {
  char cmd[3] = { ESC, 'D', 'E' };
  return sendData(3, cmd);
}

bool eDIP_I2C::displayHide() {
  char cmd[3] = { ESC, 'D', 'A' };
  return sendData(3, cmd);
}

bool eDIP_I2C::displayClear() {
  char cmd[3] = { ESC, 'D', 'L' };
  return sendData(3, cmd);
}

bool eDIP_I2C::displayFill() {
  char cmd[3] = { ESC, 'D', 'S' };
  return sendData(3, cmd);
}

bool eDIP_I2C::displayInvert() {
  char cmd[3] = { ESC, 'D', 'I' };
  return sendData(3, cmd);
}

// Text functions

bool eDIP_I2C::displayString(uint8_t align, uint8_t x, uint8_t y, const char* text) {
  char cmd[256];

  cmd[0] = ESC;
  cmd[1] = 'Z';
  switch (align) {
    case EDIP_I2C_LEFT:
      cmd[2] = 'L';
      break;
    case EDIP_I2C_CENTER:
      cmd[2] = 'C';
      break;
    case EDIP_I2C_RIGHT:
      cmd[2] = 'R';
      break;
    default:
      return false;
  }

  cmd[3] = x;
  cmd[4] = y;

  uint8_t len = 0;
  while (len < 256) {
    cmd[len + 5] = text[len];
    if (text[len] == 0)
      break;
    ++len;
  }

  return sendData(len + 6, cmd);
}

bool eDIP_I2C::setTextFont(uint8_t font) {
  char cmd[4] = { ESC, 'Z', 'F', font };
  return sendData(4, cmd);
}

bool eDIP_I2C::setTextZoom(uint8_t x, uint8_t y) {
  char cmd[5] = { ESC, 'Z', 'Z', x, y };
  return sendData(5, cmd);
}

bool eDIP_I2C::setTextDirection(uint8_t direction) {
  char cmd[4] = { ESC, 'Z', 'W', direction };
  return sendData(4, cmd);
}

bool eDIP_I2C::setTextMode(uint8_t mode) {
  char cmd[4] = { ESC, 'Z', 'V', mode };
  return sendData(4, cmd);
}

bool eDIP_I2C::setTextBlink(uint8_t blink) {
  char cmd[4] = { ESC, 'Z', 'B', blink };
  return sendData(4, cmd);
}

// Draw functions

bool eDIP_I2C::drawPoint(uint8_t x, uint8_t y) {
  char cmd[5] = { ESC, 'G', 'P', x, y };
  return sendData(5, cmd);
}

bool eDIP_I2C::drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
  char cmd[7] = { ESC, 'G', 'D', x1, y1, x2, y2 };
  return sendData(7, cmd);
}

bool eDIP_I2C::drawContinueLine(uint8_t x, uint8_t y) {
  char cmd[5] = { ESC, 'G', 'W', x, y };
  return sendData(5, cmd);
}

bool eDIP_I2C::drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
  char cmd[7] = { ESC, 'G', 'R', x1, y1, x2, y2 };
  return sendData(7, cmd);
}

bool eDIP_I2C::clearArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
  char cmd[7] = { ESC, 'R', 'L', x1, y1, x2, y2 };
  return sendData(7, cmd);
}

bool eDIP_I2C::invertArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
  char cmd[7] = { ESC, 'R', 'I', x1, y1, x2, y2 };
  return sendData(7, cmd);
}

bool eDIP_I2C::fillArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
  char cmd[7] = { ESC, 'R', 'S', x1, y1, x2, y2 };
  return sendData(7, cmd);
}

bool eDIP_I2C::fillArea(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t pattern) {
  char cmd[8] = { ESC, 'R', 'M', x1, y1, x2, y2, pattern };
  return sendData(8, cmd);
}

bool eDIP_I2C::fillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t pattern) {
  char cmd[8] = { ESC, 'R', 'O', x1, y1, x2, y2, pattern };
  return sendData(8, cmd);
}

bool eDIP_I2C::drawBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t type) {
  char cmd[8] = { ESC, 'R', 'R', x1, y1, x2, y2, type };
  return sendData(8, cmd);
}

bool eDIP_I2C::fillBox(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t type) {
  char cmd[8] = { ESC, 'R', 'T', x1, y1, x2, y2, type };
  return sendData(8, cmd);
}

bool eDIP_I2C::setDrawWidth(uint8_t x, uint8_t y) {
  char cmd[5] = { ESC, 'G', 'Z', x, y };
  return sendData(5, cmd);
}

bool eDIP_I2C::setDrawMode(uint8_t mode) {
  char cmd[4] = { ESC, 'G', 'V', mode };
  return sendData(4, cmd);
}

// Backlight functions

bool eDIP_I2C::backlightOn() {
  char cmd[4] = { ESC, 'Y', 'L', 1 };
  return sendData(4, cmd);
}

bool eDIP_I2C::backlightOff() {
  char cmd[4] = { ESC, 'Y', 'L', 0 };
  return sendData(4, cmd);
}

bool eDIP_I2C::backlight(uint8_t level) {
  char cmd[4] = { ESC, 'Y', 'H', level };
  return sendData(4, cmd);
}

// Touch functions

bool eDIP_I2C::setTouchFrame(uint8_t type) {
  char cmd[4] = { ESC, 'A', 'E', type };
  return sendData(4, cmd);
}

bool eDIP_I2C::setTouchFont(uint8_t font) {
  char cmd[4] = { ESC, 'A', 'F', font };
  return sendData(4, cmd);
}

bool eDIP_I2C::setTouchZoom(uint8_t x, uint8_t y) {
  char cmd[5] = { ESC, 'A', 'Z', x, y };
  return sendData(5, cmd);
}

bool eDIP_I2C::setMenuFont(uint8_t font) {
  char cmd[4] = { ESC, 'N', 'F', font };
  return sendData(4, cmd);
}

bool eDIP_I2C::setMenuZoom(uint8_t x, uint8_t y) {
  char cmd[5] = { ESC, 'N', 'Z', x, y };
  return sendData(5, cmd);
}

bool eDIP_I2C::setTouchResponse(uint8_t on) {
  char cmd[4] = { ESC, 'A', 'I', on };
  return sendData(4, cmd);
}

bool eDIP_I2C::createTouchKey(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t down_code, uint8_t up_code, uint8_t align, const char* text) {
  char cmd[256];

  cmd[0] = ESC;
  cmd[1] = 'A';
  cmd[2] = 'T';

  cmd[3] = x1;
  cmd[4] = y1;

  cmd[5] = x2;
  cmd[6] = y2;

  cmd[7] = down_code;
  cmd[8] = up_code;

  switch (align) {
    case EDIP_I2C_LEFT:
      cmd[9] = 'L';
      break;
    case EDIP_I2C_CENTER:
      cmd[9] = 'C';
      break;
    case EDIP_I2C_RIGHT:
      cmd[9] = 'R';
      break;
    default:
      return false;
  }

  uint8_t len = 0;
  while (len < 256) {
    cmd[len + 10] = text[len];
    if (text[len] == 0)
      break;
    ++len;
  }

  return sendData(len + 11, cmd);
}

bool eDIP_I2C::createTouchSwitch(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t down_code, uint8_t up_code, uint8_t align, const char* text) {
  char cmd[256];

  cmd[0] = ESC;
  cmd[1] = 'A';
  cmd[2] = 'K';

  cmd[3] = x1;
  cmd[4] = y1;

  cmd[5] = x2;
  cmd[6] = y2;

  cmd[7] = down_code;
  cmd[8] = up_code;

  switch (align) {
    case EDIP_I2C_LEFT:
      cmd[9] = 'L';
      break;
    case EDIP_I2C_CENTER:
      cmd[9] = 'C';
      break;
    case EDIP_I2C_RIGHT:
      cmd[9] = 'R';
      break;
    default:
      return false;
  }

  uint8_t len = 0;
  while (len < 256) {
    cmd[len + 10] = text[len];
    if (text[len] == 0)
      break;
    ++len;
  }

  return sendData(len + 11, cmd);
}

bool eDIP_I2C::createTouchMenu(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t down_code, uint8_t up_code, uint8_t menu_code, uint8_t open_dir, uint8_t align, const char* text, const char* items) {
  char cmd[256];

  cmd[0] = ESC;
  cmd[1] = 'A';
  cmd[2] = 'M';

  cmd[3] = x1;
  cmd[4] = y1;

  cmd[5] = x2;
  cmd[6] = y2;

  cmd[7] = down_code;
  cmd[8] = up_code;
  cmd[9] = menu_code;

  switch (open_dir) {
    case EDIP_I2C_LEFT:
      cmd[10] = 'L';
      break;
    case EDIP_I2C_RIGHT:
      cmd[10] = 'R';
      break;
    case EDIP_I2C_UP:
      cmd[10] = 'O';
      break;
    case EDIP_I2C_DOWN:
      cmd[10] = 'U';
      break;
    default:
      return false;
  }

  switch (align) {
    case EDIP_I2C_LEFT:
      cmd[11] = 'L';
      break;
    case EDIP_I2C_CENTER:
      cmd[11] = 'C';
      break;
    case EDIP_I2C_RIGHT:
      cmd[11] = 'R';
      break;
    default:
      return false;
  }

  uint8_t len_text = 0;
  while (len_text < 256) {
    cmd[len_text + 12] = text[len_text];
    if (text[len_text] == 0)
      break;
    ++len_text;
  }
  cmd[len_text + 12] = '|';

  uint8_t len_items = 0;
  while (len_items < 256) {
    cmd[len_items + len_text + 13] = items[len_items];
    if (items[len_items] == 0)
      break;
    ++len_items;
  }
/*
  Serial.print("Create menu [");
  Serial.print(len_items + len_text + 14);
  Serial.print("]:");
  for (uint8_t i = 0; i < len_items + len_text + 14; ++i) {
    Serial.print(" ");
    SerialPrintValue(cmd[i]);
  }
  Serial.println("");
*/
  return sendData(len_items + len_text + 14, cmd);
}

bool eDIP_I2C::setTouchSwitch(uint8_t code, uint8_t value) {
  char cmd[5] = { ESC, 'A', 'P', code, value };
  return sendData(5, cmd);
}

bool eDIP_I2C::deleteTouchArea(uint8_t code, uint8_t clear) {
  char cmd[5] = { ESC, 'A', 'L', code, clear };
  return sendData(5, cmd);
}

// Communication functions

bool eDIP_I2C::i2csend(const uint8_t code, const uint8_t len, const char* buf) {
  Wire.beginTransmission(m_address);

  uint8_t bcc = code;
  i2cwrite(code);

  bcc += len;
  i2cwrite(len);

  for (uint8_t i = 0; i < len; ++i) {
    bcc += buf[i];
    i2cwrite(buf[i]);
  }

  i2cwrite(bcc);

  Wire.endTransmission();

  unsigned long t1 = micros();

  Wire.requestFrom(m_address, (uint8_t) 1);
  uint8_t r;

  if (!i2cread(&r, m_timeout))
    return false;

  if (r == ACK)
    return true;

  if (r == NAK)
    return false;

  unsigned long t2 = micros();
  if ((t2 - t1) < m_timeout)
    delayMicroseconds(m_timeout - (t2 - t1));

  return false;
}

int eDIP_I2C::i2crecv(const uint8_t code, const uint8_t len, char* buf) {
  uint8_t bcc = 0;

  uint8_t r;
  Wire.requestFrom(m_address, (uint8_t) 1);
  if (!i2cread(&r, m_timeout))
    return -1;
  if (r != code)
    return -1;
  bcc += r;

  uint8_t l;
  Wire.requestFrom(m_address, (uint8_t) 1);
  if (!i2cread(&l, m_timeout))
    return -1;
  bcc += l;

  uint8_t rl = Wire.requestFrom(m_address, l);
  if (rl != l)
    return -1;
  for (uint8_t i = 0; i < l; ++i) {
    uint8_t tmp;
    if (!i2cread(&tmp, m_timeout))
      return -1;
    bcc += tmp;
    if (i < len)
      buf[i] = tmp;
  }
  if (l < len)
    buf[l] = 0;

  Wire.requestFrom(m_address, (uint8_t) 1);
  if (!i2cread(&r, m_timeout))
    return -1;
#ifdef EDIP_I2C_DEBUG
  Serial.print("BCC: ");
  Serial.println(bcc, HEX);
#endif
  if (r != bcc)
    return -1;

  return l;
}

static void i2cwrite(uint8_t x) {
#if ARDUINO >= 100
  Wire.write(x);
#else
  Wire.send(x);
#endif

#ifdef EDIP_I2C_DEBUG
  Serial.print("> 0x");
  SerialPrintValue(x);
  Serial.println("");
#endif
}

static bool i2cread(uint8_t *x, unsigned long timeout) {
  if (!x)
    return false;

  unsigned long t1 = micros(), t2;
  while(Wire.available() == 0) {
    delay(10);

#ifdef EDIP_I2C_DEBUG
    Serial.println("Waiting for reply");
#endif

    t2 = micros();
    if ((t2 - t1) >= timeout) {
#ifdef EDIP_I2C_DEBUG
      Serial.println("Timeout");
#endif
      return false;
    }
  }

#if ARDUINO >= 100
  *x = Wire.read();
#else
  *x = Wire.receive();
#endif

#ifdef EDIP_I2C_DEBUG
  Serial.print("< 0x");
  SerialPrintValue(*x);
  Serial.println("");
#endif

  return true;
}

static void SerialPrintValue(uint8_t x) {
  Serial.print(x, HEX);
  if (
      (x >= '0' && x <= '9') || (x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z') ||
      x == '|' || x == ' ' || x == '+' || x == '-' || x == '*' || x == '/'
     ) {
    Serial.print(" [");
    Serial.print((char) x);
    Serial.print("]");
  }
}

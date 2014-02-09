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

  Wire.requestFrom(m_address, l);
  for (uint8_t i = 0; i < l && i < len; ++i) {
    if (!i2cread((uint8_t*) &buf[i], m_timeout))
      return -1;
    bcc += buf[i];
  }

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
  Serial.println(x, HEX);
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
  Serial.println(*x, HEX);
#endif

  return true;
}

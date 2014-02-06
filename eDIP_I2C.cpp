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

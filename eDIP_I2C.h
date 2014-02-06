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

#define EDIP_I2C_VERSION "0.1"

class eDIP_I2C {
public:
  eDIP_I2C(uint8_t address);

  void begin();

  bool getProtocolSettings(uint8_t *maxPacketSize, uint8_t *actualPacketSize, uint8_t *actualTimeout);

  // RAW data function
  bool sendData(const uint8_t len, const char* buf);

private:
  bool i2csend(const uint8_t code, const uint8_t len, const char* buf);
  int i2crecv(const uint8_t code, const uint8_t len, char* buf);

  uint8_t m_address;
  unsigned long m_timeout;
  uint8_t m_packetSize;
};

#endif /* EDIP_I2C_H */

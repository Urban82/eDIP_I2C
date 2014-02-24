# eDIP_I2C

ELECTRONIC ASSEMBLY eDIP on I²C library for Arduino

This library is for use on Arduino with a EA eDIP graphic LCD display connected on I²C.

## Installation

Put a folder named `eDIP_I2C` on your `libraries` folder on your arduino workspace.

## Examples

The library contain some examples to show the usage

## Addressing

For the use with the Arduino's Wire library, the address of the display must be shifted by one:
the default address `0xDE`/`0xDF` become `0x6F`.

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

## License

Copyright (c) 2014, Danilo Treffiletti <urban82@gmail.com>  
All rights reserved.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 2.1
of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library.
If not, see <http://www.gnu.org/licenses/>.

A copy of the license is included in the files COPYING and COPYING.LESSER.

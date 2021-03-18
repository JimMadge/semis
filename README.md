# 🌱 Semis 🌱

A greenhouse climate monitor using Arduino, BME280 and ST7789

## Components

- Arduino Uno/Nano/Leonardo/Micro
- [Pimoroni BME280](https://shop.pimoroni.com/products/bme280-breakout)
- [Pimoroni 1.3" SPI Colour LCD
  (240x240)](https://shop.pimoroni.com/products/bme280-breakout) (ST7789)
- Two switch buttons

Equivalent BME280 and ST7789 breakout boards (or even building your own from
more basic components) may also work but I have not tested.

The essential features are,

- I2C interface to the BME280
- SPI interface to the ST7789
- PWM control of the ST7789 backlight

## Circuit

...

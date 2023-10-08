// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "sdkconfig.h"

#include "esp_err.h"
#include "esp_types.h"

namespace Display {

enum class Rotation {
  DEFAULT,
  // CLOCKWISE_90,
  CLOCKWISE_180,
  // CLOCKWISE_270,
};

struct Flags {
  bool transparent = false;
  bool erase = false;
};

namespace Bitmap {

typedef enum : uint8_t {
  // 1 bit per pixel, 8 pixels per uint8_t
  MONOCHROME,

  // 4 bits per pixel, 2 pixels per uint8_t
  GRAYSCALE_4_BIT,
} BitmapFormat;

}

namespace Driver {

struct PinMapSPI {
  uint8_t cs;   // Pin: Chip Select
  uint8_t rst;  // Pin: Reset
  uint8_t dc;   // Pin: Data/Command
  uint8_t sclk; // Pin: Serial Clock
  uint8_t mosi; // Pin: Master Out/Slave In
};

struct PinMap {
  PinMapSPI spi;
};

class Driver {
public:
  Driver(){};
  Driver(PinMap pins) : pins(pins){};

  PinMap pins;
  virtual uint16_t getWidth() = 0;
  virtual uint16_t getHeight() = 0;

  virtual esp_err_t sendCommands(uint8_t *commands, uint8_t bytes) = 0;

  virtual esp_err_t initializeDisplay() = 0;
  virtual esp_err_t clearBuffer() = 0;
  virtual esp_err_t sendBufferToDisplay() = 0;

  virtual esp_err_t setRotation(Rotation rotation) = 0;

  virtual void printBuffer() = 0;

  // set a single pixel
  virtual void setBufferPixel(int16_t x, int16_t y, uint16_t color) = 0;

  // set a rectangle to a single color
  virtual void setBufferBlock(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color) = 0;

  // writes a bitmap to the buffer
  virtual void writeBitmapToBuffer(int16_t x, int16_t y, uint16_t width, uint16_t height, void *bitmap,
                                   Bitmap::BitmapFormat format, uint16_t color, Flags flags = Flags()) = 0;

protected:
  // crops a block within the screen bounds, returns false if the block doesn't
  // overlap with the screen
  bool cropBlock(int16_t &x, int16_t &y, uint16_t &width, uint16_t &height);

  // writes a block of color to a buffer assuming 4 bit pixels in the
  // destination
  void write4BitColorTo4BitBuffer(uint16_t color, uint8_t *buffer, int16_t x, int16_t y, uint16_t width,
                                  uint16_t height, Flags flags = Flags());

  // writes a bitmap to a buffer assuming 1 bit pixels in the source and 4 bit
  // pixels destination using the specified color
  void write1BitBitmapTo4BitBuffer(uint8_t *bitmap, uint16_t color, uint8_t *buffer, int16_t x, int16_t y,
                                   uint16_t width, uint16_t height, Flags flags = Flags());

  // writes a bitmap to a buffer assuming 4 bit pixels in both the source and
  // destination
  void write4BitBitmapTo4BitBuffer(uint8_t *bitmap, uint8_t *buffer, int16_t x, int16_t y, uint16_t width,
                                   uint16_t height, Flags flags = Flags());
};

class SERIAL_64X64_DRIVER : public Driver {
public:
  uint16_t getWidth() { return 64; };
  uint16_t getHeight() { return 64; };

  SERIAL_64X64_DRIVER() : Driver{PinMap()} {};
  SERIAL_64X64_DRIVER(PinMap pins) : Driver{pins} {};

  esp_err_t sendCommands(uint8_t *commands, uint8_t bytes);

  esp_err_t initializeDisplay();
  esp_err_t clearBuffer();
  esp_err_t sendBufferToDisplay();

  esp_err_t setRotation(Rotation rotation);

  void printBuffer();

  void setBufferPixel(int16_t x, int16_t y, uint16_t color);
  void setBufferBlock(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);

  void writeBitmapToBuffer(int16_t x, int16_t y, uint16_t width, uint16_t height, void *bitmap,
                           Bitmap::BitmapFormat format, uint16_t color, Flags flags = Flags());

private:
  Rotation rotation = Rotation::DEFAULT;
};

class SERIAL_128X128_DRIVER : public Driver {
public:
  uint16_t getWidth() { return 128; };
  uint16_t getHeight() { return 128; };

  SERIAL_128X128_DRIVER() : Driver{PinMap()} {};
  SERIAL_128X128_DRIVER(PinMap pins) : Driver{pins} {};

  esp_err_t sendCommands(uint8_t *commands, uint8_t bytes);

  esp_err_t initializeDisplay();
  esp_err_t clearBuffer();
  esp_err_t sendBufferToDisplay();

  esp_err_t setRotation(Rotation rotation);

  void printBuffer();

  void setBufferPixel(int16_t x, int16_t y, uint16_t color);
  void setBufferBlock(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);

  void writeBitmapToBuffer(int16_t x, int16_t y, uint16_t width, uint16_t height, void *bitmap,
                           Bitmap::BitmapFormat format, uint16_t color, Flags flags = Flags());

private:
  Rotation rotation = Rotation::DEFAULT;
};

#ifndef CONFIG_IDF_TARGET_LINUX

class SSD1327_128X128_SPI_DRIVER : public Driver {
public:
  uint16_t getWidth() { return 128; };
  uint16_t getHeight() { return 128; };

  SSD1327_128X128_SPI_DRIVER(){};
  SSD1327_128X128_SPI_DRIVER(PinMap pins) : Driver{pins} {};

  esp_err_t sendCommands(uint8_t *commands, uint8_t bytes);

  esp_err_t initializeDisplay();
  esp_err_t clearBuffer();
  esp_err_t sendBufferToDisplay();

  esp_err_t setRotation(Rotation rotation);

  void printBuffer();

  void setBufferPixel(int16_t x, int16_t y, uint16_t color);
  void setBufferBlock(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);

  void writeBitmapToBuffer(int16_t x, int16_t y, uint16_t width, uint16_t height, void *bitmap,
                           Bitmap::BitmapFormat format, uint16_t color, Flags flags = Flags());
};

#endif

} // namespace Driver
} // namespace Display

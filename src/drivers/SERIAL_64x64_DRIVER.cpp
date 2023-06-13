// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstring>

#include "Driver.hpp"

namespace Display::Driver {

uint8_t SERIAL_64X64_DRIVER_BUFFER[(64 * 64 * 4) / 8] = {0};

esp_err_t SERIAL_64X64_DRIVER::sendCommands(uint8_t *commands, uint8_t bytes) { return ESP_OK; }

esp_err_t SERIAL_64X64_DRIVER::initializeDisplay() { return clearBuffer(); }

esp_err_t SERIAL_64X64_DRIVER::clearBuffer() {
  memset(SERIAL_64X64_DRIVER_BUFFER, 0, sizeof(SERIAL_64X64_DRIVER_BUFFER));
  return ESP_OK;
}

esp_err_t SERIAL_64X64_DRIVER::sendBufferToDisplay() {
  printBuffer();
  printf("\n");
  return ESP_OK;
}

void SERIAL_64X64_DRIVER::setBufferPixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || x >= 64 || y < 0 || y >= 64) {
    return;
  }

  int index = (32 * y) + (x / 2);
  if (x % 2 == 0) {
    SERIAL_64X64_DRIVER_BUFFER[index] = color << 4 | (SERIAL_64X64_DRIVER_BUFFER[index] & 0xf);
  } else {
    SERIAL_64X64_DRIVER_BUFFER[index] = (color & 0xf) | (SERIAL_64X64_DRIVER_BUFFER[index] & 0xf0);
  }
}

void SERIAL_64X64_DRIVER::setBufferBlock(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color) {
  write4BitColorTo4BitBuffer(color, SERIAL_64X64_DRIVER_BUFFER, x, y, width, height);
};

void SERIAL_64X64_DRIVER::writeBitmapToBuffer(int16_t x, int16_t y, uint16_t width, uint16_t height, void *bitmap,
                                              Bitmap::BitmapFormat format, uint16_t color, Flags flags) {
  switch (format) {
  case Bitmap::MONOCHROME:
    write1BitBitmapTo4BitBuffer((uint8_t *)bitmap, color, SERIAL_64X64_DRIVER_BUFFER, x, y, width, height, flags);
    break;
  case Bitmap::GRAYSCALE_4_BIT:
    write4BitBitmapTo4BitBuffer((uint8_t *)bitmap, SERIAL_64X64_DRIVER_BUFFER, x, y, width, height, flags);
    break;
  }
};

void SERIAL_64X64_DRIVER::printBuffer() {
  for (int y = 0; y < 64; y++) {
    for (int x = 0; x < 32; x++) {
      printf("%02x", SERIAL_64X64_DRIVER_BUFFER[(y * 32) + x]);
    }
    printf("\n");
  }
};

} // namespace Display::Driver

// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sdkconfig.h"

#ifndef CONFIG_IDF_TARGET_LINUX

#include <string.h>

#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "Driver.hpp"
#include "soc/soc_caps.h"

namespace Display::Driver {

spi_transaction_t SSD1327_128X128_DRIVER_TRANSACTION;

static spi_device_handle_t SSD1327_128X128_DRIVER_SPI_HANDLE;
spi_transaction_t SSD1327_128X128_DRIVER_SPI_TRANSACTION;

uint8_t SSD1327_128X128_DRIVER_SPI_BUFFER[(128 * 128 * 4) / 8] = {0};

esp_err_t SSD1327_128X128_SPI_DRIVER::sendCommands(uint8_t *commands, uint8_t bytes) {
  esp_err_t err;

  err = gpio_set_level((gpio_num_t)pins.spi.dc, 0); // set command mode
  if (err != ESP_OK)
    return err;

  memset(&SSD1327_128X128_DRIVER_SPI_TRANSACTION, 0, sizeof(SSD1327_128X128_DRIVER_SPI_TRANSACTION));
  SSD1327_128X128_DRIVER_SPI_TRANSACTION.length = 8 * bytes;
  SSD1327_128X128_DRIVER_SPI_TRANSACTION.tx_buffer = commands;
  SSD1327_128X128_DRIVER_SPI_TRANSACTION.rx_buffer = NULL;
  return spi_device_transmit(SSD1327_128X128_DRIVER_SPI_HANDLE, &SSD1327_128X128_DRIVER_SPI_TRANSACTION);
}

esp_err_t SSD1327_128X128_SPI_DRIVER::initializeDisplay() {
  esp_err_t err;

  // Initialize SPI
  spi_bus_config_t busConfig;
  memset(&busConfig, GPIO_NUM_NC, sizeof(spi_bus_config_t));
  busConfig.sclk_io_num = pins.spi.sclk;
  busConfig.mosi_io_num = pins.spi.mosi;
  busConfig.max_transfer_sz = 8192; // screen buffer is 8192 bytes
  busConfig.flags = 0;
  busConfig.intr_flags = 0;
  err = spi_bus_initialize(SPI2_HOST, &busConfig, SPI_DMA_CH_AUTO);
  if (err != ESP_OK)
    return err;

  spi_device_interface_config_t deviceConfig;
  memset(&deviceConfig, 0, sizeof(deviceConfig));
  deviceConfig.clock_speed_hz = 24000000;
  deviceConfig.spics_io_num = pins.spi.cs;
  deviceConfig.queue_size = 200;
  deviceConfig.pre_cb = NULL;
  deviceConfig.post_cb = NULL;
  err = spi_bus_add_device(SPI2_HOST, &deviceConfig, &SSD1327_128X128_DRIVER_SPI_HANDLE);
  if (err != ESP_OK)
    return err;

  // Configure GPIO
  uint64_t bitmask = (1ull << pins.spi.dc) | (1ull << pins.spi.rst) | (1ull << pins.spi.rst);
  gpio_config_t gpioConfig;
  gpioConfig.pin_bit_mask = bitmask;
  gpioConfig.mode = GPIO_MODE_OUTPUT;
  gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
  gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
  gpioConfig.intr_type = GPIO_INTR_DISABLE;
  err = gpio_config(&gpioConfig);
  if (err != ESP_OK)
    return err;

  // Call screen reset protocol (rst pin high -> low for at least 100 us ->
  // high)
  err = gpio_set_level((gpio_num_t)pins.spi.rst, 1);
  if (err != ESP_OK)
    return err;

  vTaskDelay(pdMS_TO_TICKS(1));

  err = gpio_set_level((gpio_num_t)pins.spi.rst, 0);
  if (err != ESP_OK)
    return err;

  vTaskDelay(pdMS_TO_TICKS(1));

  err = gpio_set_level((gpio_num_t)pins.spi.rst, 1);
  if (err != ESP_OK)
    return err;

  vTaskDelay(pdMS_TO_TICKS(1));

  // Send screen setup sequence
  uint8_t startupSequence[] = {
      0xae, // display off

      0x15, // set row start end address
      0,    // start at 0
      127,  // end at 127

      0x75, // set column start end address
      0,    // start at 0
      127,  // end at 127

      0x81, // set contrast level from 0 to 255
      127,

      0xa0,       // remap GDDRAM settings
      0b01000010, // From small to large bit (right to left)
                  // 0 - Enable Column Address Re-map
                  // 1 - Enable Nibble Re-map
                  // 2 - Enable Horizontal Address Increment
                  // 3 - Reserved (0)
                  // 4 - Enable COM Re-map
                  // 5 - Reserved (0)
                  // 6 - Enable COM Split Odd Even
                  // 7 - Reserved (0)

      0xa1, // set display start line
      0,

      0xa2, // set vertical offset
      0,

      0xa4, // set display mode to normal

      0xa8, // set mux ratio
      127,  // 127 -> 128 MUX

      0xb1,       // set phase length
      0b00100001, // High nibble is phase 2 and low nibble is phase 1

      0xb3,       // set front clock divider/oscillator frequency
      0b10100000, // High nibble for frequency and low nibble for divider ratio

      0xab,       // function selection A
      0b00000001, // smallest bit -> Enable internal VDD regulator

      0xb6, // set second precharge period, phase 3
      0b0011,

      0xbe,  // Set COM deselect voltage level
      0b111, // 0.86 x VCC

      0xbc, // set precharge voltage
      0xf,  // VCOMH

      0xd5,       // function selection B
      0b01100010, // smallest bit -> Enable External VSL, second smallest bit ->
                  // Enable Second Precharge

      // 0xb9, // set default grayscale lookup table
      0xb8, // set grayscale lookup table
      1, 2, 3, 4, 5, 7, 9, 11, 13, 15, 18, 21, 24, 27, 31,

      0xfd,       // set command lock
      0b00010010, // third smalled bit -> Lock OLED driver IC MCU interface from
                  // entering command

      0xaf, // display ON
  };

  err = sendCommands(startupSequence, sizeof(startupSequence));
  if (err != ESP_OK)
    return err;

  // clear screen
  err = clearBuffer();
  if (err != ESP_OK)
    return err;

  err = sendBufferToDisplay();
  if (err != ESP_OK)
    return err;

  return ESP_OK;
}

esp_err_t SSD1327_128X128_SPI_DRIVER::clearBuffer() {
  memset(SSD1327_128X128_DRIVER_SPI_BUFFER, 0, sizeof(SSD1327_128X128_DRIVER_SPI_BUFFER));
  return ESP_OK;
}

esp_err_t SSD1327_128X128_SPI_DRIVER::sendBufferToDisplay() {
  esp_err_t err;

  err = gpio_set_level((gpio_num_t)pins.spi.dc, 1); // set data mode
  if (err != ESP_OK)
    return err;

  memset(&SSD1327_128X128_DRIVER_SPI_TRANSACTION, 0, sizeof(SSD1327_128X128_DRIVER_SPI_TRANSACTION));
  SSD1327_128X128_DRIVER_SPI_TRANSACTION.length = 8 * sizeof(SSD1327_128X128_DRIVER_SPI_BUFFER);
  SSD1327_128X128_DRIVER_SPI_TRANSACTION.tx_buffer = SSD1327_128X128_DRIVER_SPI_BUFFER;
  SSD1327_128X128_DRIVER_SPI_TRANSACTION.rx_buffer = NULL;
  spi_device_transmit(SSD1327_128X128_DRIVER_SPI_HANDLE, &SSD1327_128X128_DRIVER_SPI_TRANSACTION);

  return ESP_OK;
}

esp_err_t SSD1327_128X128_SPI_DRIVER::setRotation(Display::Rotation rotation) {
  switch (rotation) {
  case Rotation::DEFAULT: {
    uint8_t rotateDisplay[] = {0xa0, 0b01000010};
    return sendCommands(rotateDisplay, sizeof(rotateDisplay));
    break;
  }
  case Rotation::CLOCKWISE_180: {
    uint8_t rotateDisplay[] = {0xa0, 0b01010001};
    return sendCommands(rotateDisplay, sizeof(rotateDisplay));
    break;
  }
  }
  return ESP_OK;
}

void SSD1327_128X128_SPI_DRIVER::setBufferPixel(int16_t x, int16_t y, uint16_t color) {
  if (x < 0 || x >= 128 || y < 0 || y >= 128) {
    return;
  }

  int index = (64 * y) + (x / 2);
  if (x % 2 == 0) {
    SSD1327_128X128_DRIVER_SPI_BUFFER[index] = color << 4 | (SSD1327_128X128_DRIVER_SPI_BUFFER[index] & 0xf);
  } else {
    SSD1327_128X128_DRIVER_SPI_BUFFER[index] = (color & 0xf) | (SSD1327_128X128_DRIVER_SPI_BUFFER[index] & 0xf0);
  }
}

void SSD1327_128X128_SPI_DRIVER::setBufferBlock(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color) {
  write4BitColorTo4BitBuffer(color, SSD1327_128X128_DRIVER_SPI_BUFFER, x, y, width, height);
};

void SSD1327_128X128_SPI_DRIVER::writeBitmapToBuffer(int16_t x, int16_t y, uint16_t width, uint16_t height,
                                                     void *bitmap, Bitmap::BitmapFormat format, uint16_t color,
                                                     Flags flags) {
  switch (format) {
  case Bitmap::MONOCHROME:
    write1BitBitmapTo4BitBuffer((uint8_t *)bitmap, color, SSD1327_128X128_DRIVER_SPI_BUFFER, x, y, width, height,
                                flags);
    break;
  case Bitmap::GRAYSCALE_4_BIT:
    write4BitBitmapTo4BitBuffer((uint8_t *)bitmap, SSD1327_128X128_DRIVER_SPI_BUFFER, x, y, width, height, flags);
    break;
  }
};

void SSD1327_128X128_SPI_DRIVER::printBuffer() {
  for (int y = 0; y < 128; y++) {
    for (int x = 0; x < 64; x++) {
      printf("%02x", SSD1327_128X128_DRIVER_SPI_BUFFER[(y * 64) + x]);
    }
    printf("\n");
  }
};

} // namespace Display::Driver

#endif

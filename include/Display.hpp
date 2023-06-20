// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "esp_types.h"

#include "Driver.hpp"
#include "Font.hpp"

namespace Display {

namespace Origin {
enum class Object1D {
  ENDPOINT,
  MIDPOINT,
};

enum class Object2D {
  TOP_LEFT,
  TOP_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT,
  CENTER,
};

enum class Text {
  // duplicate of Object2D
  TOP_LEFT,
  TOP_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_RIGHT,
  CENTER,

  // text specific origins
  BASELINE_LEFT,
  BASELINE_CENTER,
  BASELINE_RIGHT,
};
} // namespace Origin

class Display {
public:
  Display(){};
  Display(Driver::Driver *driver) : driver(driver){};

  esp_err_t setup();
  esp_err_t clear();
  esp_err_t update();

  esp_err_t setRotation(Rotation rotation);

  void printBuffer() { driver->printBuffer(); };

  void drawPixel(int16_t x, int16_t y, uint16_t color);

  void drawLine(int16_t xStart, int16_t yStart, int16_t xEnd, int16_t yEnd, uint16_t color);
  void drawLine(Origin::Object1D origin, int16_t x, int16_t y, double length, double angle, uint16_t color);

  void drawCircle(Origin::Object2D origin, int16_t x, int16_t y, uint16_t diameter, uint16_t color);

  void drawRectangle(Origin::Object2D origin, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);
  void fillRectangle(Origin::Object2D origin, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);

  void drawBitmap(Origin::Object2D origin, int16_t x, int16_t y, uint16_t width, uint16_t height,
                  Bitmap::BitmapFormat format, void *bitmap, Flags flags = Flags());
  void drawBitmap(Origin::Object2D origin, int16_t x, int16_t y, uint16_t width, uint16_t height,
                  Bitmap::BitmapFormat format, void *bitmap, uint16_t color, Flags flags = Flags());

  void drawText(Origin::Text origin, int16_t x, int16_t y, uint8_t *font, char *text, uint16_t color,
                Flags flags = Flags());
  void getTextSize(uint8_t *fontData, char *text, uint16_t &width, uint16_t &height);

  Driver::Driver *driver;

private:
  void drawCircleWithEvenDiameterFromTopLeftCorner(int16_t x, int16_t y, uint16_t diameter, uint16_t color);
  void drawCircleWithOddDiameterFromCenter(int16_t x, int16_t y, uint16_t diameter, uint16_t color);

  void shiftOrigin2DToTopLeft(Origin::Object2D origin, int16_t &x, int16_t &y, uint16_t width, uint16_t height);

  // read the first UTF-8 character from a string and advance the string pointer
  // however many bytes the character spans
  uint16_t readUTF8Char(char *&string);

  // also sets the font origin X and Y offset from the top left corner of the
  // string bounding box
  void getTextSize(uint8_t *fontData, char *text, uint16_t &width, uint16_t &height, int16_t &originXOffset,
                   int16_t &originYOffset, uint16_t &baselineLength);
};

} // namespace Display

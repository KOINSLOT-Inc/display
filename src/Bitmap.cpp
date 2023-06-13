// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Display.hpp"

namespace Display {

void Display::drawBitmap(Origin::Object2D origin, int16_t x, int16_t y, uint16_t width, uint16_t height,
                         Bitmap::BitmapFormat format, void *bitmap, Flags flags) {
  shiftOrigin2DToTopLeft(origin, x, y, width, height);
  driver->writeBitmapToBuffer(x, y, width, height, bitmap, format, 0xffff, flags);
};

void Display::drawBitmap(Origin::Object2D origin, int16_t x, int16_t y, uint16_t width, uint16_t height,
                         Bitmap::BitmapFormat format, void *bitmap, uint16_t color, Flags flags) {
  shiftOrigin2DToTopLeft(origin, x, y, width, height);
  driver->writeBitmapToBuffer(x, y, width, height, bitmap, format, color, flags);
};

} // namespace Display

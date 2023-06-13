// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Display.hpp"

namespace Display {

void Display::drawRectangle(Origin::Object2D origin, int16_t x, int16_t y, uint16_t width, uint16_t height,
                            uint16_t color) {
  shiftOrigin2DToTopLeft(origin, x, y, width, height);
  driver->setBufferBlock(x, y, width, 1, color);              // top line
  driver->setBufferBlock(x, y + height - 1, width, 1, color); // bottom line
  driver->setBufferBlock(x, y, 1, height, color);             // left line
  driver->setBufferBlock(x + width - 1, y, 1, height, color); // right line
};

void Display::fillRectangle(Origin::Object2D origin, int16_t x, int16_t y, uint16_t width, uint16_t height,
                            uint16_t color) {
  shiftOrigin2DToTopLeft(origin, x, y, width, height);
  driver->setBufferBlock(x, y, width, height, color);
};

} // namespace Display

// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Display.hpp"

namespace Display {

esp_err_t Display::setup() { return driver->initializeDisplay(); }

esp_err_t Display::clear() { return driver->clearBuffer(); }

esp_err_t Display::update() { return driver->sendBufferToDisplay(); }

void Display::drawPixel(int16_t x, int16_t y, uint16_t color) { driver->setBufferPixel(x, y, color); }

void Display::shiftOrigin2DToTopLeft(Origin::Object2D origin, int16_t &x, int16_t &y, uint16_t width, uint16_t height) {
  switch (origin) {
  case Origin::Object2D::TOP_LEFT:
    break;
  case Origin::Object2D::TOP_RIGHT:
    x -= width - 1;
    break;
  case Origin::Object2D::BOTTOM_LEFT:
    y -= height - 1;
    break;
  case Origin::Object2D::BOTTOM_RIGHT:
    x -= width - 1;
    y -= height - 1;
    break;
  case Origin::Object2D::CENTER:
    // similarly to circles, where there is no pixel center we bias to the
    // bottom right so that the left bound is at `x
    // - (width / 2)` and the upper bound is at `y - (height / 2)`.
    x -= width / 2;
    y -= height / 2;
    break;
  }
}

} // namespace Display

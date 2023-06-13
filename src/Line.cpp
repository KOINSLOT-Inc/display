// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "math.h"

#include "Display.hpp"
#include <cmath>

namespace Display {

void Display::drawLine(int16_t xStart, int16_t yStart, int16_t xEnd, int16_t yEnd, uint16_t color) {
  if (yStart == yEnd) {  // horizontal line
    if (xEnd < xStart) { // setBufferBlock draws left-to-right so make sure xEnd
                         // is >= xStart
      std::swap(xEnd, xStart);
      std::swap(yEnd, yStart);
    }
    driver->setBufferBlock(xStart, yStart, xEnd - xStart + 1, 1, color);
    return;
  }

  if (xStart == xEnd) {  // vertical line
    if (yEnd < yStart) { // setBufferBlock draws top-to-bottom so make sure yEnd
                         // is >= yStart
      std::swap(xEnd, xStart);
      std::swap(yEnd, yStart);
    }
    driver->setBufferBlock(xStart, yStart, 1, yEnd - yStart + 1, color);
    return;
  }

  // for sloped lines use Bresenham's Algorithm with integer arithmetic
  // <https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm>
  int16_t dX = abs(xEnd - xStart), xStep = xStart < xEnd ? 1 : -1;
  int16_t dY = -1 * abs(yEnd - yStart), yStep = yStart < yEnd ? 1 : -1;
  int16_t xHead = xStart, yHead = yStart;
  int16_t error = dX + dY;

  while (true) {
    driver->setBufferPixel(xHead, yHead, color);

    if (xHead == xEnd && yHead == yEnd)
      break;

    if (2 * error >= dY) {
      if (xHead == xEnd)
        break;

      error += dY;
      xHead += xStep;
    }

    if (2 * error <= dX) {
      if (yHead == yEnd)
        break;

      error += dX;
      yHead += yStep;
    }
  }
}

void Display::drawLine(Origin::Object1D origin, int16_t x, int16_t y, double length, double angle, uint16_t color) {
  int16_t xStart = 0, yStart = 0, xEnd = 0, yEnd = 0;

  // multiply y deltas by -1 since our y-axis is inverted compared to standard
  // cartesian coordinates
  switch (origin) {
  case Origin::Object1D::ENDPOINT:
    xStart = x;
    yStart = y;
    xEnd = round(xStart + length * cos(angle));
    yEnd = round(yStart + -1 * length * sin(angle));
    break;
  case Origin::Object1D::MIDPOINT:
    xStart = round(x - 0.5 * length * cos(angle));
    yStart = round(y - -1 * 0.5 * length * sin(angle));
    xEnd = round(x + 0.5 * length * cos(angle));
    yEnd = round(y + -1 * 0.5 * length * sin(angle));
    break;
  }

  drawLine(xStart, yStart, xEnd, yEnd, color);
};

} // namespace Display

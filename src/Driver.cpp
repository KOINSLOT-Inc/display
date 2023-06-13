// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Driver.hpp"
#include "Display.hpp"

namespace Display::Driver {

bool Driver::cropBlock(int16_t &x, int16_t &y, uint16_t &width, uint16_t &height) {
  if (x > (getWidth() - 1) || y > (getHeight() - 1) || x + width - 1 < 0 || y + height - 1 < 0)
    return false;

  if (x < 0) {
    width += x;
    x = 0;
  }

  if ((x + width - 1) > (getWidth() - 1)) {
    width = getWidth() - x;
  }

  if (y < 0) {
    height += y;
    y = 0;
  }

  if ((y + height - 1) > (getWidth() - 1)) {
    height = getWidth() - y;
  }

  return true;
}

void Driver::write1BitBitmapTo4BitBuffer(uint8_t *bitmap, uint16_t color, uint8_t *buffer, int16_t x, int16_t y,
                                         uint16_t width, uint16_t height, Flags flags) {
  uint16_t bitmapX = 0, bitmapY = 0, bitmapWidth = width;

  if (x < 0)
    bitmapX -= x; // left edge of bitmap is off screen

  if (y < 0)
    bitmapY -= y; // top edge of bitmap is off screen

  if (!cropBlock(x, y, width, height))
    return; // no overlap between bitmap and screen

  // set screen cursor to the position where the bitmap will be written
  buffer += (y * getWidth() + x) / 2;

  // precompute colors
  uint8_t lowNibbleColor = 0x0f & color;
  uint8_t highNibbleColor = 0xf0 & (color << 4);

  // set bitmap cursor to the beginning of the part of the bitmap we want to
  // write
  bitmap += (bitmapY * bitmapWidth + bitmapX) / 8;

  // left edge is the low nibble of a uint8_t buffer entry
  bool splitLeft = x % 2 != 0;

  // right edge is the high nibble of a uint8_t buffer entry
  bool splitRight = (x + width - 1) % 2 == 0;

  // number of unsplit bytes in each row to write
  uint16_t innerBytes = (width - (splitLeft ? 1 : 0) - (splitRight ? 1 : 0)) / 2;

  //                                   bitmap width
  //                   |-------------------------------------------|
  //    buffer ->  ________ ________ ________ ________ ________ ________
  //
  //                   |--| |---------------------------------| |--|
  //                    ^                                         ^
  //  splitLeft = true _|             innerBytes = 4              |_ splitRight
  //  = true

  uint16_t bufferWrapDistance = (getWidth() / 2) - innerBytes - (splitLeft ? 1 : 0) - (splitRight ? 1 : 0);
  uint16_t bitmapWrapDistance = ((bitmapWidth - width) / 8);
  uint16_t bitmapWrapRemainder = ((bitmapWidth - width) % 8);

  uint8_t bit = 7 - (bitmapY * bitmapWidth + bitmapX) % 8; // zero indexed
  for (int16_t j = 0; j < height; j++) {

    if (splitLeft) { // fill left edge

      if (((*bitmap >> bit) & 0b1) != 0) {
        *buffer = (*buffer & 0xf0) | lowNibbleColor;
      } else {
        if (!flags.transparent) {
          *buffer &= 0xf0;
        }
      }

      buffer++;

      if (bit == 0) {
        bitmap++;
        bit = 7;
      } else {
        bit--;
      }
    }

    for (int16_t i = 0; i < innerBytes; i++) { // fill inner span

      // high nibble
      if (((*bitmap >> bit) & 0b1) != 0) {
        *buffer = (*buffer & 0x0f) | highNibbleColor;
      } else {
        if (!flags.transparent) {
          *buffer &= 0x0f;
        }
      }

      if (bit == 0) {
        bitmap++;
        bit = 7;
      } else {
        bit--;
      }

      // low nibble
      if (((*bitmap >> bit) & 0b1) != 0) {
        *buffer = (*buffer & 0xf0) | lowNibbleColor;
      } else {
        if (!flags.transparent) {
          *buffer &= 0xf0;
        }
      }

      if (bit == 0) {
        bitmap++;
        bit = 7;
      } else {
        bit--;
      }

      buffer++;
    }

    if (splitRight) { // fill right edge

      if (((*bitmap >> bit) & 0b1) != 0) {
        *buffer = (*buffer & 0x0f) | highNibbleColor;
      } else {
        if (!flags.transparent) {
          *buffer &= 0x0f;
        }
      }

      buffer++;

      if (bit == 0) {
        bitmap++;
        bit = 7;
      } else {
        bit--;
      }
    }

    buffer += bufferWrapDistance;
    bitmap += bitmapWrapDistance;
    if (bitmapWrapRemainder > bit) {
      bitmap++;
      bit += 8 - bitmapWrapRemainder;
    } else {
      bit -= bitmapWrapRemainder;
    }
  }
};

void Driver::write4BitColorTo4BitBuffer(uint16_t color, uint8_t *buffer, int16_t x, int16_t y, uint16_t width,
                                        uint16_t height, Flags flags) {
  if (!cropBlock(x, y, width, height))
    return; // no overlap between block and screen

  // increment to top left corner of the cropped block
  buffer += (y * getWidth() + x) / 2;

  // precompute colors
  uint8_t lowNibbleColor = 0x0f & color;
  uint8_t highNibbleColor = 0xf0 & (color << 4);
  uint8_t innerColor = lowNibbleColor | highNibbleColor;

  // left edge is the low nibble of a uint8_t buffer entry
  bool splitLeft = x % 2 != 0;

  // right edge is the high nibble of a uint8_t buffer entry
  bool splitRight = (x + width - 1) % 2 == 0;
  // full bytes between split edges
  uint16_t innerBytes = (width - (splitLeft ? 1 : 0) - (splitRight ? 1 : 0)) / 2;

  // distance to increment buffer to wrap from end of block to beginning of
  // block on next line
  uint16_t wrapDistance = (getWidth() / 2) - innerBytes - (splitLeft ? 1 : 0) - (splitRight ? 1 : 0);

  for (int16_t j = 0; j < height; j++) {

    if (splitLeft) { // fill left edge
      *buffer = (*buffer & 0xf0) | lowNibbleColor;
      buffer++;
    }

    for (int16_t i = 0; i < innerBytes; i++) { // fill inner span
      *buffer = innerColor;
      buffer++;
    }

    if (splitRight) { // fill right edge
      *buffer = (*buffer & 0x0f) | highNibbleColor;
      buffer++;
    }

    buffer += wrapDistance;
  }
};

void Driver::write4BitBitmapTo4BitBuffer(uint8_t *bitmap, uint8_t *buffer, int16_t x, int16_t y, uint16_t width,
                                         uint16_t height, Flags flags) {
  uint16_t bitmapX = 0, bitmapY = 0, bitmapWidth = width;

  if (x < 0)
    bitmapX -= x; // left edge of bitmap is off screen

  if (y < 0)
    bitmapY -= y; // top edge of bitmap is off screen

  if (!cropBlock(x, y, width, height))
    return; // no overlap between bitmap and screen

  // set screen cursor to the position where the bitmap will be written
  buffer += (y * getWidth() + x) / 2;

  // set bitmap cursor to the beginning of the part of the bitmap we want to
  // write
  bitmap += (bitmapY * bitmapWidth + bitmapX) / 2;

  // left edge is the low nibble of a uint8_t buffer entry
  bool splitLeft = x % 2 != 0;

  // right edge is the high nibble of a uint8_t buffer entry
  bool splitRight = (x + width - 1) % 2 == 0;

  // number of unsplit bytes in each row to write
  uint16_t innerBytes = (width - (splitLeft ? 1 : 0) - (splitRight ? 1 : 0)) / 2;

  //                                   bitmap width
  //                   |-------------------------------------------|
  //    buffer ->  ________ ________ ________ ________ ________ ________
  //
  //                   |--| |---------------------------------| |--|
  //                    ^                                         ^
  //  splitLeft = true _|             innerBytes = 4              |_ splitRight
  //  = true

  // compute same flags for bitmap range
  bool bitmapSplitLeft = bitmapX % 2 != 0;
  bool bitmapSplitRight = (bitmapX + width - 1) % 2 == 0;

  // do inner bytes of bitmap and buffer align
  bool unaligned = splitLeft != bitmapSplitLeft;

  // unaligned = true: inner bytes of buffer and bitmap don't line up, even if
  // one or both ends do
  //   bitmap ->  -------- -------- -------- -------- -------- --------
  //   buffer ->  ---- -------- -------- -------- -------- -------- --------
  //
  // unaligned = false: inner bytes of buffer and bitmap do line up, even if one
  // or both ends don't
  //   bitmap ->  -------- -------- -------- -------- -------- --------
  //   buffer ->      ---- -------- -------- -------- -------- ----
  //
  // distance to increment buffer/bitmap to wrap from end of block to beginning
  // of block on next line

  uint16_t bufferWrapDistance = (getWidth() / 2) - innerBytes - (splitLeft ? 1 : 0) - (splitRight ? 1 : 0);
  uint16_t bitmapWrapDistance = (bitmapWidth / 2) - innerBytes - (bitmapSplitLeft ? 1 : 0) - (bitmapSplitRight ? 1 : 0);

  if (unaligned) { // we have to bitshift all copies
    for (int16_t j = 0; j < height; j++) {

      if (splitLeft) { // fill left edge, we can assume the bitmapSplitLeft ==
                       // false
        if (*bitmap >> 4 != 0) {
          *buffer = (*buffer & 0xf0) | (*bitmap >> 4);
        } else {
          if (!flags.transparent) {
            *buffer &= 0xf0;
          }
        }
        buffer++;
      }

      for (int16_t i = 0; i < innerBytes; i++) { // fill inner span
        // high nibble
        if (*bitmap << 4 != 0) {
          *buffer = (*bitmap & 0x0f) | (*bitmap << 4);
        } else {
          if (!flags.transparent) {
            *buffer &= 0x0f;
          }
        }
        bitmap++;

        // low nibble
        if (*bitmap >> 4 != 0) {
          *buffer = (*bitmap & 0xf0) | (*bitmap >> 4);
        } else {
          if (!flags.transparent) {
            *buffer &= 0xf0;
          }
        }
        buffer++;
      }

      if (splitRight) { // fill right edge, we can assume the bitmapSplitRight
                        // == false
        if (*bitmap << 4 != 0) {
          *buffer = (*buffer & 0x0f) | (*bitmap << 4);
        } else {
          if (!flags.transparent) {
            *buffer &= 0x0f;
          }
        }
        buffer++;
      }

      bitmap++;

      buffer += bufferWrapDistance;
      bitmap += bitmapWrapDistance;
    }
  } else { // we can directly copy the inner bytes

    for (int16_t j = 0; j < height; j++) {

      if (splitLeft) { // fill left edge, we can assume the bitmapSplitLeft ==
                       // true
        if ((*bitmap & 0x0f) != 0) {
          *buffer = (*buffer & 0xf0) | (*bitmap & 0x0f);
        } else {
          if (!flags.transparent) {
            *buffer &= 0xf0;
          }
        }

        buffer++;
        bitmap++;
      }

      for (int16_t i = 0; i < innerBytes; i++) { // fill inner span
        if (flags.transparent) {
          // high nibble
          if ((*bitmap & 0xf0) != 0)
            *buffer = (*buffer & 0x0f) | (*bitmap & 0xf0);

          // low nibble
          if ((*bitmap & 0x0f) != 0)
            *buffer = (*buffer & 0xf0) | (*bitmap & 0x0f);
        } else {
          *buffer = *bitmap;
        }

        buffer++;
        bitmap++;
      }

      if (splitRight) { // fill right edge, we can assume the bitmapSplitRight
                        // == true
        if ((*bitmap & 0xf0) != 0) {
          *buffer = (*buffer & 0x0f) | (*bitmap & 0xf0);
        } else {
          if (!flags.transparent) {
            *buffer &= 0xf;
          }
        }

        buffer++;
        bitmap++;
      }

      buffer += bufferWrapDistance;
      bitmap += bitmapWrapDistance;
    }
  }
};

} // namespace Display::Driver

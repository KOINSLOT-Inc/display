// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "Fonts.hpp"

namespace Display::Font {

class Character {
public:
  CHARACTER_CODE_TYPE code = 0;
  CHARACTER_BYTES_TYPE bytes = 0;
  CHARACTER_DEVICE_WIDTH_X_TYPE deviceWidthX = 0;
  CHARACTER_DEVICE_WIDTH_Y_TYPE deviceWidthY = 0;
  CHARACTER_BBX_WIDTH_TYPE bbxWidth = 0;
  CHARACTER_BBX_HEIGHT_TYPE bbxHeight = 0;
  CHARACTER_BBX_X_OFFSET_TYPE bbxXOffset = 0;
  CHARACTER_BBX_Y_OFFSET_TYPE bbxYOffset = 0;

  uint8_t *bitmap = nullptr;

  Character(uint8_t *character);
  Character(){};
};

class Font {
public:
  FONT_SIZE_TYPE size;
  FONT_CHARACTERS_TYPE numCharacters;
  FONT_BOUNDING_BOX_WIDTH_TYPE boundingBoxWidth;
  FONT_BOUNDING_BOX_HEIGHT_TYPE boundingBoxHeight;
  FONT_BOUNDING_BOX_X_OFFSET_TYPE boundingBoxXOffset;
  FONT_BOUNDING_BOX_Y_OFFSET_TYPE boundingBoxYOffset;
  FONT_ASCENT_TYPE ascent;
  FONT_DESCENT_TYPE descent;

  uint8_t *firstCharacter;

  Font(uint8_t *font);

  Character getCharacter(uint16_t character);
};

} // namespace Display::Font

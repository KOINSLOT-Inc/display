// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "esp_types.h"

namespace Display::Font {

#define FONT_SIZE 0
#define FONT_SIZE_TYPE uint8_t
#define FONT_CHARACTERS 1
#define FONT_CHARACTERS_TYPE uint16_t
#define FONT_BOUNDING_BOX_WIDTH 3
#define FONT_BOUNDING_BOX_WIDTH_TYPE uint8_t
#define FONT_BOUNDING_BOX_HEIGHT 4
#define FONT_BOUNDING_BOX_HEIGHT_TYPE uint8_t
#define FONT_BOUNDING_BOX_X_OFFSET 5
#define FONT_BOUNDING_BOX_X_OFFSET_TYPE int8_t
#define FONT_BOUNDING_BOX_Y_OFFSET 6
#define FONT_BOUNDING_BOX_Y_OFFSET_TYPE int8_t
#define FONT_ASCENT 7
#define FONT_ASCENT_TYPE uint8_t
#define FONT_DESCENT 8
#define FONT_DESCENT_TYPE uint8_t
#define FONT_FIRST_CHARACTER 9
#define CHARACTER_CODE 0
#define CHARACTER_CODE_TYPE uint16_t
#define CHARACTER_BYTES 2
#define CHARACTER_BYTES_TYPE uint16_t
#define CHARACTER_DEVICE_WIDTH_X 4
#define CHARACTER_DEVICE_WIDTH_X_TYPE uint8_t
#define CHARACTER_DEVICE_WIDTH_Y 5
#define CHARACTER_DEVICE_WIDTH_Y_TYPE uint8_t
#define CHARACTER_BBX_WIDTH 6
#define CHARACTER_BBX_WIDTH_TYPE uint8_t
#define CHARACTER_BBX_HEIGHT 7
#define CHARACTER_BBX_HEIGHT_TYPE uint8_t
#define CHARACTER_BBX_X_OFFSET 8
#define CHARACTER_BBX_X_OFFSET_TYPE int8_t
#define CHARACTER_BBX_Y_OFFSET 9
#define CHARACTER_BBX_Y_OFFSET_TYPE int8_t
#define CHARACTER_BITMAP 10

// bailleul
//   - Copyright (c) 2019 Yomli  Copyright (c) 1850 Bailleul et Cie  Copyright (c) 1800 Justus Erich Walbaum"
//   - Size: 8
//   - Characters: 232
extern uint8_t bailleul_8_pt[3698];

// bailleul
//   - Copyright (c) 2019 Yomli  Copyright (c) 1850 Bailleul et Cie  Copyright (c) 1800 Justus Erich Walbaum"
//   - Size: 12
//   - Characters: 232
extern uint8_t bailleul_12_pt[5324];

// bailleul
//   - Copyright (c) 2019 Yomli  Copyright (c) 1850 Bailleul et Cie  Copyright (c) 1800 Justus Erich Walbaum"
//   - Size: 16
//   - Characters: 232
extern uint8_t bailleul_16_pt[7002];

// bailleul_bold
//   - Copyright (c) 2019 Yomli  Copyright (c) 1850 Bailleul et Cie  Copyright (c) 1800 Justus Erich Walbaum"
//   - Size: 8
//   - Characters: 232
extern uint8_t bailleul_bold_8_pt[3850];

// bailleul_bold
//   - Copyright (c) 2019 Yomli  Copyright (c) 1850 Bailleul et Cie  Copyright (c) 1800 Justus Erich Walbaum"
//   - Size: 12
//   - Characters: 232
extern uint8_t bailleul_bold_12_pt[5584];

// bailleul_bold
//   - Copyright (c) 2019 Yomli  Copyright (c) 1850 Bailleul et Cie  Copyright (c) 1800 Justus Erich Walbaum"
//   - Size: 16
//   - Characters: 232
extern uint8_t bailleul_bold_16_pt[7418];

// intel_one_mono
//   - (C) 2023 Intel Corporation"
//   - Size: 8
//   - Characters: 622
extern uint8_t intel_one_mono_8_pt[9558];

// intel_one_mono
//   - (C) 2023 Intel Corporation"
//   - Size: 12
//   - Characters: 622
extern uint8_t intel_one_mono_12_pt[13771];

// intel_one_mono
//   - (C) 2023 Intel Corporation"
//   - Size: 16
//   - Characters: 622
extern uint8_t intel_one_mono_16_pt[19232];

} // namespace Display::Font
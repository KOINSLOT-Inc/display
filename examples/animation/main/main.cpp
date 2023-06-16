// SPDX-FileCopyrightText: 2023 KOINSLOT, Inc.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstdio>

#include "Display.hpp"

Display::Display display;

extern "C" void app_main(void) {
  Display::Driver::SERIAL_64X64_DRIVER displayDriver = Display::Driver::SERIAL_64X64_DRIVER();
  display = Display::Display(&displayDriver);
  display.setup();

  const char *text[] = {"k", "y", "w", "y"};
  int index = 0;

  while (true) {
    display.drawRectangle(Display::Origin::Object2D::TOP_LEFT, 7, 7, 50, 50, 0xff);
    display.drawRectangle(Display::Origin::Object2D::TOP_LEFT, 17, 17, 32, 32, 0xff);
    display.drawText(Display::Origin::Text::CENTER, 32, 32, Display::Font::bailleul_16_pt,
                     const_cast<char *>(text[index]), 0xff, {.transparent = true});
    display.update();

    index = (index + 1) % 4;
    vTaskDelay(pdMS_TO_TICKS(500));
    display.clear();
  }
}

/* -*- mode: c++ -*-
 * Kaleidoscope - Firmware for computer input devices
 * Copyright (C) 2022  Keyboard.io, Inc.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "kaleidoscope/plugin/LEDBloom.h"

#include <Arduino.h>                       // for PSTR, strcmp_P, F, __FlashStringHelper
#include <Kaleidoscope-FocusSerial.h>      // for Focus, FocusSerial
#include <stdint.h>                        // for uint8_t, uint16_t

#include "kaleidoscope/device/device.h"         // for VirtualProps::Storage, Base<>::Storage
#include "kaleidoscope/event_handler_result.h"  // for EventHandlerResult, EventHandlerResult::OK
#include "kaleidoscope/plugin/LEDControl.h"     // for LEDControl

namespace kaleidoscope {
namespace plugin {

EventHandlerResult LEDBloom::onSetup() {
  return EventHandlerResult::OK;
}

EventHandlerResult LEDBloom::onFocusEvent(const char *command) {
  const char *cmd = PSTR("led.bloom");
  if (::Focus.handleHelp(command, cmd))
    return EventHandlerResult::OK;

  if (strcmp_P(command, cmd) != 0)
    return EventHandlerResult::OK;

  // allocate space for the indices
  uint8_t idx1;
  uint8_t idx2;
  uint8_t idx3;
  // read the value of the index
  ::Focus.read(idx1);
  ::Focus.read(idx2);
  ::Focus.read(idx3);
  // now we read the cRGB to set the indices
  cRGB c;
  // focus-send led.at 3 integers 63 or less
  ::Focus.read(c);
  ::LEDControl.setCrgbAt(idx1,c);
  ::LEDControl.setCrgbAt(idx2,c);
  ::LEDControl.setCrgbAt(idx3,c);

  return EventHandlerResult::EVENT_CONSUMED;
}

EventHandlerResult LEDBloom::onNameQuery() {
  return ::Focus.sendName(F("LEDBloom"));
}

}  // namespace plugin
}  // namespace kaleidoscope

kaleidoscope::plugin::LEDBloom LEDBloom;

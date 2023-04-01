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

#include "kaleidoscope/plugin/LEDControlConfig.h"

#include <Arduino.h>                       // for PSTR, strcmp_P, F, __FlashStringHelper
#include <Kaleidoscope-EEPROM-Settings.h>  // for EEPROMSettings
#include <Kaleidoscope-FocusSerial.h>      // for Focus, FocusSerial
#include <stdint.h>                        // for uint8_t, uint16_t

#include "kaleidoscope/Runtime.h"               // for Runtime, Runtime_
#include "kaleidoscope/device/device.h"         // for VirtualProps::Storage, Base<>::Storage
#include "kaleidoscope/event_handler_result.h"  // for EventHandlerResult, EventHandlerResult::OK
#include "kaleidoscope/plugin/LEDControl.h"     // for LEDControl

namespace kaleidoscope {
namespace plugin {

uint16_t LEDControlConfig::settings_base_;
struct LEDControlConfig::settings LEDControlConfig::settings_;

EventHandlerResult LEDControlConfig::onSetup() {
  settings_base_ = ::EEPROMSettings.requestSlice(sizeof(settings_));

  Runtime.storage().get(settings_base_, settings_);

  // We do not need to treat uninitialized slices in any special way, because
  // uninitialized defaults to `255`, which happens to be our desired default
  // brightness, too.
  ::LEDControl.setBrightness(settings_.brightness);

  return EventHandlerResult::OK;
}

EventHandlerResult LEDControlConfig::onFocusEvent(const char *command) {
  const char *cmd = PSTR("led.at");
  if (::Focus.handleHelp(command, cmd))
    return EventHandlerResult::OK;
  // looks like this checks whether part of the string matches "led.at"
  // Is splitting on spaces magically done for us somewhere else?
  if (strcmp_P(command, cmd) != 0)
    return EventHandlerResult::OK;

  // allocate space for the index
  uint8_t idx;
  // read the value of the index
  ::Focus.read(idx);
  // if that's all, return the value at that index
  if (::Focus.isEOL()) {
    cRGB c = ::LEDControl.getCrgbAt(idx);
    ::Focus.send(c.r);
    ::Focus.send(c.g);
    ::Focus.send(c.b);
  } else { // if there's more to the line
    // now we read the cRGB to set
    cRGB c;
    // this works on led_mode.default 14 only so far?
    // focus-send led.at 10 250 250 250
    ::Focus.read(c);
    ::LEDControl.setCrgbAt(idx,c);
  }

  return EventHandlerResult::EVENT_CONSUMED;
}

EventHandlerResult LEDControlConfig::onNameQuery() {
  return ::Focus.sendName(F("LEDControlConfig"));
}

}  // namespace plugin
}  // namespace kaleidoscope

kaleidoscope::plugin::LEDControlConfig LEDControlConfig;

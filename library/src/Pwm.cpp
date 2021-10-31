/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.
// Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc

#include <printer/Printer.hpp>

#include "hal/Pwm.hpp"

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Pwm::Attributes &a) {
  return printer.key("flags", var::NumberString(a.o_flags()))
    .key("frequency", var::NumberString(a.frequency()))
    .key("period", var::NumberString(a.period()))
    .key(
      "channel0",
      var::NumberString().format(
        "%d.%d",
        a.pin_assignment_channel0().port,
        a.pin_assignment_channel0().pin))
    .key(
      "channel1",
      var::NumberString().format(
        "%d.%d",
        a.pin_assignment_channel1().port,
        a.pin_assignment_channel1().pin))
    .key(
      "channel2",
      var::NumberString().format(
        "%d.%d",
        a.pin_assignment_channel2().port,
        a.pin_assignment_channel2().pin))
    .key(
      "channel3",
      var::NumberString().format(
        "%d.%d",
        a.pin_assignment_channel3().port,
        a.pin_assignment_channel3().pin));
}

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Pwm::Info &a) {
  return printer.key("flags", var::NumberString(static_cast<u32>(a.o_flags())))
    .key("events", var::NumberString(a.o_events()));
}

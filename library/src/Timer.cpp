// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/Timer.hpp"

printer::Printer &printer::operator<<(
  printer::Printer &printer,
  const hal::Timer::Attributes &a) {
  printer.key("flags", var::NumberString(static_cast<u32>(a.o_flags())));
  printer.key("frequency", var::NumberString(a.frequency()));
  printer.key("period", var::NumberString(a.period()));
  printer.key(
    "channel0",
    var::NumberString().format("%d.%d", a.channel0().port, a.channel0().pin));
  printer.key(
    "channel1",
    var::NumberString().format("%d.%d", a.channel1().port, a.channel1().pin));
  printer.key(
    "channel2",
    var::NumberString().format("%d.%d", a.channel2().port, a.channel2().pin));
  printer.key(
    "channel3",
    var::NumberString().format("%d.%d", a.channel3().port, a.channel3().pin));
  return printer;
}

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Timer::Info &a) {
  printer.key("flags", var::NumberString(static_cast<u32>(a.o_flags())));
  printer.key("events", var::NumberString(a.o_events()));
  return printer;
}

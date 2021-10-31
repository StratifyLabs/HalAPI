// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/I2S.hpp"

using namespace hal;

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::I2S::Attributes &a) {
  return printer.key("frequency", var::NumberString(a.frequency()))
    .key("scl", var::NumberString().format("%d.%d", a.sck().port, a.sck().pin))
    .key(
      "sdin",
      var::NumberString().format("%d.%d", a.sdin().port, a.sdin().pin))
    .key(
      "sdout",
      var::NumberString().format("%d.%d", a.sdout().port, a.sdout().pin))
    .key("mck", var::NumberString().format("%d.%d", a.mck().port, a.mck().pin))
    .key("ws", var::NumberString().format("%d.%d", a.ws().port, a.ws().pin))
    .key("slot", var::NumberString(a.slot()))
    .key("flags", var::NumberString(a.o_flags(), "%0x08lX"));
}

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::I2S::Info &a) {
  printer.key("frequency", var::NumberString(a.frequency()))
    .key("events", var::NumberString(a.o_events(), "0x%08lx"))
    .key("flags", var::NumberString(a.o_flags(), "0x%08lx"));
  return printer;
}

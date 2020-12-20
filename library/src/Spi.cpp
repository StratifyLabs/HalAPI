// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/Spi.hpp"

using namespace hal;

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Spi::Attributes &a) {
  printer.key("frequency", var::NumberString(a.frequency()));
  printer.key("width", var::NumberString(a.width()));
  printer.key("miso",
              var::NumberString().format("%d.%d", a.miso().port, a.miso().pin));
  printer.key("mosi",
              var::NumberString().format("%d.%d", a.mosi().port, a.mosi().pin));
  printer.key("sck",
              var::NumberString().format("%d.%d", a.sck().port, a.sck().pin));
  printer.key("cs",
              var::NumberString().format("%d.%d", a.cs().port, a.cs().pin));
  return printer;
}

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Spi::Info &a) {
  printer.key("flags", var::NumberString(static_cast<u32>(a.o_flags())));
  printer.key("events", var::NumberString(a.o_events()));
  return printer;
}

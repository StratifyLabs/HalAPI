// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/Gpio.hpp"

using namespace hal;

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Gpio::Attributes &a) {
  printer.key("flags", var::NumberString(static_cast<u32>(a.o_flags())));
  printer.key("pinmask", var::NumberString(a.o_pinmask()));
  return printer;
}

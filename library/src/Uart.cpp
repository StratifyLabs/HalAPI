// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/Uart.hpp"

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Uart::Attributes &a) {  
  printer.key("frequency", var::NumberString(a.frequency()));
  printer.key("width", var::NumberString(a.width()));
  printer.key("rx", var::String().format("%d.%d", a.rx().port, a.rx().pin));
  printer.key("tx", var::String().format("%d.%d", a.tx().port, a.tx().pin));
  printer.key("rts", var::String().format("%d.%d", a.rts().port, a.rts().pin));
  printer.key("cts", var::String().format("%d.%d", a.cts().port, a.cts().pin));
  return printer;
}

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Uart::Info &a) {
  printer.key_bool("valid", a.is_valid());
  printer.key("sizeReady", var::NumberString(a.size_ready()));
  printer.key("size", var::NumberString(a.size()));
  return printer;
}


// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/Uart.hpp"

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Uart::Attributes &a) {
  return printer.key("frequency", var::NumberString(a.frequency()))
    .key("width", var::NumberString(a.width()))
    .key("rx", var::String().format("%d.%d", a.rx().port, a.rx().pin))
    .key("tx", var::String().format("%d.%d", a.tx().port, a.tx().pin))
    .key("rts", var::String().format("%d.%d", a.rts().port, a.rts().pin))
    .key("cts", var::String().format("%d.%d", a.cts().port, a.cts().pin));
}

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Uart::Info &a) {
  return printer.key_bool("valid", a.is_valid())
    .key("sizeReady", var::NumberString(a.size_ready()))
    .key("size", var::NumberString(a.size()));
}

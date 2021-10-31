// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/Usb.hpp"

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Usb::Attributes &a) {
  return printer.key("frequency", var::NumberString(a.frequency()))
    .key("dp", var::String().format("%d.%d", a.dp().port, a.dp().pin))
    .key("dm", var::String().format("%d.%d", a.dm().port, a.dm().pin))
    .key("id", var::String().format("%d.%d", a.id().port, a.id().pin))
    .key("vbus", var::String().format("%d.%d", a.vbus().port, a.vbus().pin));
}

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Usb::Info &a) {
  return printer.key("flags", var::NumberString(a.o_flags(), "0x%08x"))
    .key("events", var::NumberString(a.o_events(), "0x%08x"));
}

// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/ByteBuffer.hpp"

using namespace hal;

printer::Printer &printer::operator<<(
  printer::Printer &printer,
  const hal::ByteBuffer::Attributes &a) {
  return printer
    .key_bool("initialize", a.flags() & hal::ByteBuffer::Flags::initialize)
    .key_bool("finalize", a.flags() & hal::ByteBuffer::Flags::exit)
    .key_bool("writeBlock", a.flags() & hal::ByteBuffer::Flags::set_writeblock)
    .key_bool("isOverflow", a.flags() & hal::ByteBuffer::Flags::is_overflow);
}

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::ByteBuffer::Info &a) {
  return printer.key("sizeReady", var::NumberString(a.size_ready()))
    .key("size", var::NumberString(a.size()))
    .key_bool("isOverflow", a.is_overflow());
}

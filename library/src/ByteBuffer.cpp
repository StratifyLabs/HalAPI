/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md for rights.
// Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/ByteBuffer.hpp"

using namespace hal;

printer::Printer &printer::operator<<(printer::Printer &printer,
                                      const hal::ByteBuffer::Attributes &a) {
  printer.key_bool("initialize",
                   a.flags() & hal::ByteBuffer::Flags::initialize);
  printer.key_bool("finalize", a.flags() & hal::ByteBuffer::Flags::exit);
  printer.key_bool("writeBlock",
                   a.flags() & hal::ByteBuffer::Flags::set_writeblock);
  printer.key_bool("isOverflow",
                   a.flags() & hal::ByteBuffer::Flags::is_overflow);
  return printer;
}

printer::Printer &printer::operator<<(printer::Printer &printer,
                                      const hal::ByteBuffer::Info &a) {
  printer.key("sizeReady", var::NumberString(a.size_ready()));
  printer.key("size", var::NumberString(a.size()));
  printer.key_bool("isOverflow", a.is_overflow());
  return printer;
}

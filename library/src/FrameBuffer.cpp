// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md
#include <printer/Printer.hpp>

#include "hal/FrameBuffer.hpp"

using namespace hal;

printer::Printer &printer::operator<<(
  printer::Printer &printer,
  const hal::FrameBuffer::Attributes &a) {
  return printer
    .key_bool("isOverflow", a.flags() & hal::FrameBuffer::Flags::is_overflow)
    .key_bool(
      "setWriteblock",
      a.flags() & hal::FrameBuffer::Flags::set_writeblock)
    .key_bool(
      "isNotifyWrite",
      a.flags() & hal::FrameBuffer::Flags::is_notify_write)
    .key_bool(
      "isNotifyWrite",
      a.flags() & hal::FrameBuffer::Flags::is_notify_read);
}

printer::Printer &printer::operator<<(
  printer::Printer &printer,
  const hal::FrameBuffer::Info &a) {
  return printer.key_bool("isValid", a.is_valid())
    .key("frameSize", var::NumberString(a.frame_size()))
    .key("frameCount", var::NumberString(a.frame_count()))
    .key("frameCountReady", var::NumberString(a.frame_count_ready()))
    .key("size", var::NumberString(a.size()))
    .key_bool("isOverflow", a.is_overflow());
}

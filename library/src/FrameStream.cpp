// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md
#include <printer/Printer.hpp>

#include "hal/FrameStream.hpp"

using namespace hal;

printer::Printer &printer::operator<<(
  printer::Printer &printer,
  const hal::FrameStream::Attributes &a) {
  return printer.key_bool("isStart", a.flags() & hal::FrameStream::Flags::start)
    .key_bool("isStop", a.flags() & hal::FrameStream::Flags::stop)
    .key_bool("isFlush", a.flags() & hal::FrameStream::Flags::flush);
}

printer::Printer &printer::operator<<(
  printer::Printer &printer,
  const hal::FrameStream::Info &a) {
  return printer.key_bool("isStopped", a.is_stopped())
    .key_bool("isRunning", a.is_running())
    .key_bool("isReset", a.is_reset())
    .key_bool("isValid", a.is_valid())
    .object("transmit", a.transmit())
    .object("receive", a.receive());
}

printer::Printer &printer::operator<<(
  printer::Printer &printer,
  const hal::FrameStream::ChannelInfo &a) {
  return printer.key("accessCount", var::NumberString(a.access_count()))
    .key("error", var::NumberString(a.error()));
}
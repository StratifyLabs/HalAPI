// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/Adc.hpp"

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Adc::Attributes &a) {
  return printer.key("flags", var::NumberString(static_cast<u32>(a.o_flags())))
    .key("frequency", var::NumberString(a.frequency()))
    .key("width", var::NumberString(a.width()))
    .key("channelCount", var::NumberString(a.channel_count()))
    .key("samplingTime", var::NumberString(a.sampling_time()))
    .key("rank", var::NumberString(a.rank()))
    .key("channel", var::NumberString(a.channel()))
    .key(
      "channel0",
      var::NumberString().format("%d.%d", a.channel0().port, a.channel0().pin))
    .key(
      "channel1",
      var::NumberString().format("%d.%d", a.channel1().port, a.channel1().pin))
    .key(
      "channel2",
      var::NumberString().format("%d.%d", a.channel2().port, a.channel2().pin))
    .key(
      "channel3",
      var::NumberString().format("%d.%d", a.channel3().port, a.channel3().pin));
}

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Adc::Info &a) {
  return printer.key("flags", var::NumberString(static_cast<u32>(a.o_flags())))
    .key("events", var::NumberString(a.o_events()));
}

/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.
#include "hal/Drive.hpp"
#include "printer/Printer.hpp"

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Drive::Info &a) {
  return printer.key("addressableSize", var::NumberString(a.addressable_size()))
    .key("eraseBlockSize", var::NumberString(a.erase_block_size()))
    .key("eraseBlockTime", var::NumberString(a.erase_block_time(), "%ldus"))
    .key(
      "eraseDeviceTime",
      var::NumberString(a.erase_device_time().milliseconds(), "%ldms"))
    .key("frequency", var::NumberString(a.frequency()))
    .key("events", var::NumberString(a.o_events(), "0x%lX"))
    .key("flags", var::NumberString(a.o_flags(), "0x%lX"))
    .key("pageProgramSize", var::NumberString(a.page_program_size()))
    .key("writeBlockSize", var::NumberString(a.write_block_size()))
    .key("writeBlockCount", var::NumberString(a.num_write_blocks()));
}

printer::Printer &printer::operator<<(
  printer::Printer &printer,
  const hal::Drive::Attributes &a) {
  return printer.key("oFlags", var::NumberString(a.o_flags(), "0x%lX"))
    .key("start", var::NumberString(a.start()))
    .key("end", var::NumberString(a.end()));
}

using namespace hal;

const Drive &Drive::erase_device() const {
  const auto info = get_info();
  if (info.flags() & Flags::erase_device) {
    return set_attributes(Attributes().set_flags(Flags::erase_device));
  }

  u32 address = 0;
  while (address < info.size()) {

    address += set_attributes(Attributes()
                                .set_flags(Flags::erase_blocks)
                                .set_start(address)
                                .set_end(info.write_block_size()))
                 .return_value();

    while (is_busy() && is_success()) {
      chrono::wait(info.erase_block_time());
    }
  }
  return *this;
}

const Drive &Drive::erase_blocks(u32 start_address, u32 end_address) const {
  return set_attributes(Attributes()
                          .set_flags(Flags::erase_blocks)
                          .set_start(start_address)
                          .set_end(end_address));
}

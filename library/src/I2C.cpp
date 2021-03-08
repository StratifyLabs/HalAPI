// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#include <printer/Printer.hpp>
#include <var/StackString.hpp>

#include "hal/I2C.hpp"

using namespace hal;

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::I2C::Attributes &a) {
  printer.key("frequency", var::NumberString(a.frequency()));
  printer.key("scl",
              var::NumberString().format("%d.%d", a.scl().port, a.scl().pin));
  printer.key("sda",
              var::NumberString().format("%d.%d", a.sda().port, a.sda().pin));
  return printer;
}

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::I2C::Info &a) {
  printer.key("frequency", var::NumberString(a.frequency()));
  printer.key("error", var::NumberString(a.error()));
  return printer;
}


I2C::ScanResult I2C::scan() const {
  ScanResult result;
  result.fill(0);
  for(u8 addr = 1; addr < 128; addr++){
    char c = 0;
    api::ErrorGuard eg;
    prepare(addr, Flags::prepare_data).read(&c, 1);
    if( return_value() == 1 ){
      result.at(addr) = addr;
    }
  }
  return result;
}

//
// Created by Tyler Gilbert on 10/29/22.
//

#include "hal/Pin.hpp"

using namespace hal;

const Pin &Pin::set_output(GpioFlags::Flags flags) const & {
  auto attr = pio_attr_t{
    .o_flags = static_cast<u32>(flags | Flags::set_output),
    .o_pinmask = m_pinmask};
  return ioctl(I_PIO_SETATTR, &attr);
}

Pin::PulseScope::PulseScope(Pin &pin, Pin::IsActiveHigh is_active_high)
  : context{Context{&pin, is_active_high}, &deleter} {
  pin.set_value(is_active_high == IsActiveHigh::yes);
}

void Pin::PulseScope::deleter(Pin::PulseScope::Context *context) {
  context->pin->set_value(context->is_active_high == IsActiveHigh::no);
}

bool Pin::is_floating(GpioFlags::Flags o_restore_flags) const {
  bool result = true;
  set_input(Flags::is_pullup);
  if (!get_value()) {
    result = false; // pulled high but driven low
  }

  if (result) {
    set_input(Flags::is_pulldown);
    if (get_value()) {
      result = false; // pulled low but driven high
    }
  }
  set_input(o_restore_flags);
  return result;
}

bool Pin::get_value() const {
  u32 value;
  ioctl(I_PIO_GET, &value);
  return (value & m_pinmask) != 0;
}

const Pin &Pin::wait(const chrono::MicroTime &a) const & {
  chrono::wait(a);
  return *this;
}

const Pin &Pin::set_input(GpioFlags::Flags flags) const & {
  auto attr = pio_attr_t{
    .o_flags = static_cast<u32>(flags | Flags::set_input),
    .o_pinmask = m_pinmask};
  return ioctl(I_PIO_SETATTR, &attr);
}


/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.

#ifndef HALAPI_HAL_PIN_HPP_
#define HALAPI_HAL_PIN_HPP_

#include <var/String.hpp>

#include "Gpio.hpp"

namespace hal {

class Pin : public DeviceAccess<Pin>, public GpioFlags {
public:
  static mcu_pin_t from_string(const var::StringView port_pin) {
    var::StringViewList token_list = port_pin.split(".");
    mcu_pin_t result;
    result.port = 0xff;
    result.pin = 0xff;
    if (token_list.count() == 2) {
      result.port = token_list.at(0).to_integer();
      result.pin = token_list.at(1).to_integer();
    }
    return result;
  }

  Pin(const var::StringView device, u8 pin) : DeviceAccess(device) {
    m_pinmask = (1 << pin);
  }

  Pin &set_input(Flags flags = Flags::null) {
    pio_attr_t attr;
    attr.o_pinmask = m_pinmask;
    attr.o_flags = static_cast<u32>(flags | Flags::set_input);
    return ioctl(I_PIO_SETATTR, &attr);
  }

  Pin &set_output(Flags flags = Flags::null) {
    pio_attr_t attr;
    attr.o_pinmask = m_pinmask;
    attr.o_flags = static_cast<u32>(flags | Flags::set_output);
    return ioctl(I_PIO_SETATTR, &attr);
  }

  const Pin &wait(const chrono::MicroTime &a) const {
    chrono::wait(a);
    return *this;
  }

  const Pin &set_value(bool value = true) const {
    return ioctl(value ? I_PIO_SETMASK : I_PIO_CLRMASK,
                 MCU_INT_CAST(m_pinmask));
  }

  bool get_value() const {
    return ioctl(I_PIO_GET, nullptr).return_value() & m_pinmask != 0;
  }

  bool is_floating(Flags o_restore_flags = Flags::is_float) {
    bool result = true;
    set_input(Flags::is_pullup);
    if (get_value() == false) {
      result = false; // pulled high but driven low
    }

    if (result) {
      set_input(Flags::is_pulldown);
      if (get_value() == true) {
        result = false; // pulled low but driven high
      }
    }
    set_input(o_restore_flags);
    return result;
  }

private:
  u32 m_pinmask;
};

} // namespace hal

#endif /* HALAPI_HAL_PIN_HPP_ */

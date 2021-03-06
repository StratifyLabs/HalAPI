// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

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

  Pin(const var::StringView device,
      u8 pin FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess(device, fs::OpenMode::read_write()
                                 FSAPI_LINK_INHERIT_DRIVER_LAST) {
    m_pinmask = (1 << pin);
  }

  explicit Pin(const mcu_pin_t pin FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess("/dev/pio" & var::NumberString(pin.port),
                     fs::OpenMode::read_write()
                     FSAPI_LINK_INHERIT_DRIVER_LAST) {
    m_pinmask = (1 << pin.pin);
  }

  Pin() {}
  Pin(const Pin &a) = delete;
  Pin &operator=(const Pin &a) = delete;

  Pin(Pin &&a) { swap(a); }
  Pin &operator=(Pin &&a) {
    swap(a);
    return *this;
  }

  const Pin &set_input(Flags flags = Flags::null) const {
    pio_attr_t attr;
    attr.o_pinmask = m_pinmask;
    attr.o_flags = static_cast<u32>(flags | Flags::set_input);
    return ioctl(I_PIO_SETATTR, &attr);
  }

  Pin &set_input(Flags flags = Flags::null){
    return API_CONST_CAST_SELF(Pin, set_input, flags);
  }

  const Pin &set_output(Flags flags = Flags::null) const {
    pio_attr_t attr;
    attr.o_pinmask = m_pinmask;
    attr.o_flags = static_cast<u32>(flags | Flags::set_output);
    return ioctl(I_PIO_SETATTR, &attr);
  }

  Pin &set_output(Flags flags = Flags::null){
    return API_CONST_CAST_SELF(Pin, set_output, flags);
  }

  const Pin &wait(const chrono::MicroTime &a) const {
    chrono::wait(a);
    return *this;
  }

  Pin &wait(const chrono::MicroTime &a){
    chrono::wait(a);
    return *this;
  }

  const Pin &set_value(bool value = true) const {
    return ioctl(value ? I_PIO_SETMASK : I_PIO_CLRMASK,
                 MCU_INT_CAST(m_pinmask));
  }

  Pin &set_value(bool value = true) {
    return API_CONST_CAST_SELF(Pin, set_value, value);
  }


  bool get_value() const {
    u32 value;
    ioctl(I_PIO_GET, &value);
    return (value & m_pinmask) != 0;
  }

  bool is_floating(Flags o_restore_flags = Flags::is_float) const {
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

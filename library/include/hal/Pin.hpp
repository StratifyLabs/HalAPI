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
    mcu_pin_t result = {.port = 0xff, .pin = 0xff};
    if (token_list.count() == 2) {
      result.port = token_list.at(0).to_integer();
      result.pin = token_list.at(1).to_integer();
    }
    return result;
  }

  Pin() = default;
  Pin(
    const var::StringView device,
    u8 pin FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(
      device,
      fs::OpenMode::read_write() FSAPI_LINK_INHERIT_DRIVER_LAST),
      m_pinmask(1 << pin) {}

  explicit Pin(const mcu_pin_t pin FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(
      "/dev/pio" & var::NumberString(pin.port),
      fs::OpenMode::read_write() FSAPI_LINK_INHERIT_DRIVER_LAST),
      m_pinmask(1 << pin.pin) {}


  const Pin &set_input(Flags flags = Flags::null) const &;
  Pin &set_input(Flags flags = Flags::null) & {
    return API_CONST_CAST_SELF(Pin, set_input, flags);
  }

  Pin &&set_input(Flags flags = Flags::null) && {
    return std::move(set_input(flags));
  }

  const Pin &set_output(Flags flags = Flags::null) const &;
  Pin &set_output(Flags flags = Flags::null) & {
    return API_CONST_CAST_SELF(Pin, set_output, flags);
  }
  Pin &&set_output(Flags flags = Flags::null) && {
    return std::move(set_output(flags));
  }

  const Pin &wait(const chrono::MicroTime &a) const &;
  Pin &wait(const chrono::MicroTime &a) & {
    return API_CONST_CAST_SELF(Pin, wait, a);
  }

  Pin &&wait(const chrono::MicroTime &a) && { return std::move(wait(a)); }

  const Pin &set_value(bool value = true) const & {
    return ioctl(
      value ? I_PIO_SETMASK : I_PIO_CLRMASK,
      MCU_INT_CAST(m_pinmask));
  }

  Pin &set_value(bool value = true) & {
    return API_CONST_CAST_SELF(Pin, set_value, value);
  }
  Pin &&set_value(bool value = true) && { return std::move(set_value(value)); }

  API_NO_DISCARD bool get_value() const;
  API_NO_DISCARD bool
  is_floating(Flags o_restore_flags = Flags::is_float) const;

  enum class IsActiveHigh { no, yes };

  class PulseScope {
    struct Context {
      Pin *pin;
      IsActiveHigh is_active_high;
    };
    static void deleter(Context *context);
    api::SystemResource<Context, decltype(&deleter)> context
      = {Context{}, nullptr};

  public:
    explicit PulseScope(
      Pin &pin,
      IsActiveHigh is_active_high = IsActiveHigh::yes);
  };

private:
  u32 m_pinmask{};
};

} // namespace hal

#endif /* HALAPI_HAL_PIN_HPP_ */

// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_GPIO_HPP_
#define HALAPI_HAL_GPIO_HPP_

#include <sos/dev/pio.h>

#include "Device.hpp"

namespace hal {

class GpioFlags {
public:
  enum class Flags {
    null = 0,
    set_input = PIO_FLAG_SET_INPUT,
    set_output = PIO_FLAG_SET_OUTPUT,
    is_pullup = PIO_FLAG_IS_PULLUP,
    is_pulldown = PIO_FLAG_IS_PULLDOWN,
    is_repeater = PIO_FLAG_IS_REPEATER,
    is_float = PIO_FLAG_IS_FLOAT,
    is_speed_low = PIO_FLAG_IS_SPEED_LOW,
    is_speed_medium = PIO_FLAG_IS_SPEED_MEDIUM,
    is_speed_high = PIO_FLAG_IS_SPEED_HIGH,
    is_speed_blazing = PIO_FLAG_IS_SPEED_BLAZING,
    is_opendrain = PIO_FLAG_IS_OPENDRAIN,
    is_hysteresis = PIO_FLAG_IS_HYSTERESIS,
    is_direction_only = PIO_FLAG_IS_DIRONLY,
    is_analog = PIO_FLAG_IS_ANALOG,
    is_invert = PIO_FLAG_IS_INVERT,
    is_filter = PIO_FLAG_IS_FILTER,
    set = PIO_FLAG_SET,
    clear = PIO_FLAG_CLEAR,
    assign = PIO_FLAG_ASSIGN
  };
};

API_OR_NAMED_FLAGS_OPERATOR(GpioFlags, Flags)

class Gpio : public DeviceAccess<Gpio>, public GpioFlags {
public:
  class Attributes {
  public:
    Attributes() : m_attributes{} {}

    Attributes(Flags o_flags, u32 o_pinmask) {
      set_flags(o_flags);
      set_pinmask(o_pinmask);
    }

    u32 o_flags() const { return m_attributes.o_flags; }
    Flags flags() const { return Flags(m_attributes.o_flags); }
    u32 o_pinmask() const { return m_attributes.o_pinmask; }

    Attributes &set_flags(Flags value) {
      m_attributes.o_flags = static_cast<u32>(value);
      return *this;
    }

    Attributes &set_pinmask(u32 value) {
      m_attributes.o_pinmask = value;
      return *this;
    }

    pio_attr_t *attributes() { return &m_attributes; }
    const pio_attr_t *attributes() const { return &m_attributes; }

  private:
    friend class Gpio;
    pio_attr_t m_attributes;
  };

  Gpio(const var::StringView device FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess(device, DEVICE_OPEN_MODE FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Gpio() {}
  Gpio(const Gpio &a) = delete;
  Gpio &operator=(const Gpio &a) = delete;

  Gpio(Gpio &&a) { swap(a); }
  Gpio &operator=(Gpio &&a) {
    swap(a);
    return *this;
  }

  Gpio &set_attributes(const Attributes &attributes) {
    return ioctl(I_PIO_SETATTR, (void *)&attributes.m_attributes);
  }
  Gpio &set_mask(u32 mask) { return ioctl(I_PIO_SETMASK, MCU_INT_CAST(mask)); }
  Gpio &clear_mask(u32 mask) {
    return ioctl(I_PIO_CLRMASK, MCU_INT_CAST(mask));
  }
  Gpio &assign(u32 value) {
    return set_attributes(Attributes(Flags::assign, value));
  }

  Gpio &set_value(u32 value) { return ioctl(I_PIO_SET, MCU_INT_CAST(value)); }
  u32 get_value() const {
    u32 value;
    ioctl(I_PIO_GET, &value);
    return value;
  }


};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::Gpio::Attributes &a);
} // namespace printer

#endif /* HALAPI_HAL_GPIO_HPP_ */

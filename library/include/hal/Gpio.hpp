/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.

#ifndef HALAPI_HAL_GPIO_HPP_
#define HALAPI_HAL_GPIO_HPP_

#include <sos/dev/pio.h>

#include "Device.hpp"

namespace hal {

class GpioFlags {
public:
  enum class Flags {
    null = 0,
    set_input /*! Sets the pinmask as input */ = PIO_FLAG_SET_INPUT,
    set_output /*! Sets the pinmask as output */ = PIO_FLAG_SET_OUTPUT,
    is_pullup /*! When setting as input, enables the pullup */
    = PIO_FLAG_IS_PULLUP,
    is_pulldown /*! When setting as input, enables the pulldown */
    = PIO_FLAG_IS_PULLDOWN,
    is_repeater /*! When setting as input, enables repeater mode (if
                        available) */
    = PIO_FLAG_IS_REPEATER,
    is_float /*! When setting as input, enables floating mode (no pullup or
                     pulldown) */
    = PIO_FLAG_IS_FLOAT,
    is_speed_low /*! When setting as output, enables low speed mode */
    = PIO_FLAG_IS_SPEED_LOW,
    is_speed_medium /*! When setting as output, enables medium speed mode
                     */
    = PIO_FLAG_IS_SPEED_MEDIUM,
    is_speed_high /*! When setting as output, enables high speed mode */
    = PIO_FLAG_IS_SPEED_HIGH,
    is_speed_blazing /*! When setting as output, enables blazing speed mode
                      */
    = PIO_FLAG_IS_SPEED_BLAZING,
    is_opendrain /*! When setting as output, enables open drain mode */
    = PIO_FLAG_IS_OPENDRAIN,
    is_hysteresis /*! When setting as input, enables hysteresis */
    = PIO_FLAG_IS_HYSTERESIS,
    is_direction_only /*! When set, only affects the direction withouth
                              setting other attributes */
    = PIO_FLAG_IS_DIRONLY,
    is_analog /*! When setting as input, sets pin as analog mode */
    = PIO_FLAG_IS_ANALOG,
    is_invert /*! See \ref PIO_FLAG_IS_INVERT */ = PIO_FLAG_IS_INVERT,
    is_filter /*! See \ref PIO_FLAG_IS_FILTER */ = PIO_FLAG_IS_FILTER,
    set /*! Assigns the value of the port to one if the corresponding bit
                in the pin mask is set. */
    = PIO_FLAG_SET,
    clear /*! Assigns the value of the port to zero if the corresponding
                  bit in the pin mask is set. */
    = PIO_FLAG_CLEAR,
    assign /*! Assigns the value of the port to the value of the pin mask.
            */
    = PIO_FLAG_ASSIGN
  };
};

API_OR_NAMED_FLAGS_OPERATOR(GpioFlags, Flags)

class Gpio : public DeviceAccess<Gpio>, public GpioFlags {
public:
  class Attributes {
  public:
    Attributes(Flags o_flags, u32 o_pinmask) {
      set_flags(o_flags);
      set_pinmask(o_pinmask);
    }

    Flags o_flags() const { return static_cast<Flags>(m_attributes.o_flags); }
    u32 o_pinmask() const { return m_attributes.o_pinmask; }

    Attributes &set_flags(Flags value) {
      m_attributes.o_flags = static_cast<u32>(value);
      return *this;
    }

    Attributes &set_pinmask(u32 value) {
      m_attributes.o_pinmask = value;
      return *this;
    }

  private:
    friend class Gpio;
    pio_attr_t m_attributes;
  };

  Gpio(const var::StringView device) : DeviceAccess(device) {}

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
  u32 get_value() { return ioctl(I_PIO_GET, nullptr).return_value(); }

private:
  const Device *m_device;
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::Gpio::Attributes &a);
} // namespace printer

#endif /* HALAPI_HAL_GPIO_HPP_ */

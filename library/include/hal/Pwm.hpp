// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_PWM_HPP_
#define HALAPI_HAL_PWM_HPP_

#include <sos/dev/pwm.h>

#include "Device.hpp"

namespace hal {

struct PwmFlags {
public:
  enum class Flags {
    set_timer = PWM_FLAG_SET_TIMER,
    is_active_high = PWM_FLAG_IS_ACTIVE_HIGH,
    is_active_low = PWM_FLAG_IS_ACTIVE_LOW,
    set_channels = PWM_FLAG_SET_CHANNELS,
    clear_channels = PWM_FLAG_CLEAR_CHANNELS,
    is_enabled = PWM_FLAG_IS_ENABLED
  };
};

API_OR_NAMED_FLAGS_OPERATOR(PwmFlags, Flags)

class Pwm : public DeviceAccess<Pwm>, public PwmFlags {
public:
  class Info {
  public:
    Info() = default;
    explicit Info(const pwm_info_t &info) { m_info = info; }
    API_NO_DISCARD bool is_valid() const { return m_info.o_flags != 0; }
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_flags)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)
  private:
    friend class Pwm;
    pwm_info_t m_info{};
  };

  class Attributes {
  public:
    // period, channel

    explicit Attributes(const pwm_attr_t &attr) : m_attributes(attr) {}
    Attributes() {
      set_flags(Flags::set_timer);
      set_frequency(1000000);
      m_attributes.channel = mcu_channel(-1, -1);
      m_attributes.period = 1000;
      var::View(m_attributes.pin_assignment).fill<u8>(0xff);
    }

    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      u32,
      attributes,
      frequency,
      freq)

    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, period)

    API_NO_DISCARD Flags flags() const { return Flags(m_attributes.o_flags); }
    API_NO_DISCARD u32 o_flags() const { return m_attributes.o_flags; }

    Attributes &set_flags(Flags value) {
      m_attributes.o_flags = static_cast<u32>(value);
      return *this;
    }

    API_NO_DISCARD mcu_channel_t channel() const {
      return m_attributes.channel;
    }

    Attributes &set_channel(u32 loc, u32 value) {
      m_attributes.channel = mcu_channel(loc, value);
      return *this;
    }

    Attributes &set_channel(const mcu_channel_t &channel) {
      m_attributes.channel = channel;
      return *this;
    }

    API_NO_DISCARD mcu_pin_t pin_assignment_channel0() const {
      return m_attributes.pin_assignment.channel[0];
    }
    API_NO_DISCARD mcu_pin_t pin_assignment_channel1() const {
      return m_attributes.pin_assignment.channel[1];
    }
    API_NO_DISCARD mcu_pin_t pin_assignment_channel2() const {
      return m_attributes.pin_assignment.channel[2];
    }
    API_NO_DISCARD mcu_pin_t pin_assignment_channel3() const {
      return m_attributes.pin_assignment.channel[3];
    }

    Attributes &set_pin_assignment_channel0(mcu_pin_t pin) {
      m_attributes.pin_assignment.channel[0] = pin;
      return *this;
    }

    Attributes &set_pin_assignment_channel1(mcu_pin_t pin) {
      m_attributes.pin_assignment.channel[1] = pin;
      return *this;
    }

    Attributes &set_pin_assignment_channel2(mcu_pin_t pin) {
      m_attributes.pin_assignment.channel[2] = pin;
      return *this;
    }

    Attributes &set_pin_assignment_channel3(mcu_pin_t pin) {
      m_attributes.pin_assignment.channel[3] = pin;
      return *this;
    }

    pwm_attr_t *attributes() { return &m_attributes; }
    API_NO_DISCARD const pwm_attr_t *attributes() const {
      return &m_attributes;
    }

  private:
    pwm_attr_t m_attributes{};
  };

  explicit Pwm(
    const var::StringView device,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Pwm() = default;

  const Pwm &set_attributes(const Attributes &attr) const {
    ioctl(I_PWM_SETATTR, (void *)&attr);
    return *this;
  }
  Pwm &set_attributes(const Attributes &attr) {
    return API_CONST_CAST_SELF(Pwm, set_attributes, attr);
  }

  const Pwm &set_channel(const mcu_channel_t &channel) const {
    ioctl(I_PWM_SETCHANNEL, (void *)&channel);
    return *this;
  }
  Pwm &set_channel(const mcu_channel_t &channel) {
    return API_CONST_CAST_SELF(Pwm, set_channel, channel);
  }

  API_NO_DISCARD int get_channel(u32 loc) const {
    mcu_channel_t channel = {loc, 0};
    ioctl(I_PWM_GETCHANNEL, &channel);
    return channel.value;
  }

  const Pwm &enable() const {
    ioctl(I_PWM_ENABLE);
    return *this;
  }

  Pwm &enable() { return API_CONST_CAST_SELF(Pwm, enable); }

  const Pwm &disable() const {
    ioctl(I_PWM_DISABLE);
    return *this;
  }

  Pwm &disable() { return API_CONST_CAST_SELF(Pwm, disable); }

  API_NO_DISCARD u32 get_value() const {
    u32 result = 0;
    ioctl(I_PWM_GET, &result);
    return result;
  }

  Pwm &set_value(u32 value) { return ioctl(I_PWM_SET, &value); }

  const Pwm &set_value(u32 value) const { return ioctl(I_PWM_SET, &value); }

private:
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::Pwm::Attributes &a);
Printer &operator<<(Printer &printer, const hal::Pwm::Info &a);
} // namespace printer

#endif /* HALAPI_HAL_PWM_HPP_ */

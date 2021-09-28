// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_TIMER_HPP_
#define HALAPI_HAL_TIMER_HPP_

#include <sos/dev/tmr.h>

#include "Device.hpp"

namespace hal {

struct TimerFlags {
  enum class Flags {
    null = 0,
    set_timer = TMR_FLAG_SET_TIMER,
    is_source_cpu = TMR_FLAG_IS_SOURCE_CPU,
    is_source_ic0 = TMR_FLAG_IS_SOURCE_IC0,
    is_source_ic1 = TMR_FLAG_IS_SOURCE_IC1,
    is_source_ic2 = TMR_FLAG_IS_SOURCE_IC2,
    is_source_ic3 = TMR_FLAG_IS_SOURCE_IC3,
    is_source_edgerising = TMR_FLAG_IS_SOURCE_EDGERISING,
    is_source_edgefalling = TMR_FLAG_IS_SOURCE_EDGEFALLING,
    is_source_edgeboth = TMR_FLAG_IS_SOURCE_EDGEBOTH,
    is_source_countdown = TMR_FLAG_IS_SOURCE_COUNTDOWN,
    is_auto_reload = TMR_FLAG_IS_AUTO_RELOAD,
    set_channel = TMR_FLAG_SET_CHANNEL,
    is_channel_edgerising = TMR_FLAG_IS_CHANNEL_EDGERISING,
    is_channel_edgefalling = TMR_FLAG_IS_CHANNEL_EDGEFALLING,
    is_channel_edgeboth = TMR_FLAG_IS_CHANNEL_EDGEBOTH,
    is_channel_stop_on_reset = TMR_FLAG_IS_CHANNEL_STOP_ON_RESET,
    is_channel_reset_on_match = TMR_FLAG_IS_CHANNEL_RESET_ON_MATCH,
    is_channel_stop_on_match = TMR_FLAG_IS_CHANNEL_STOP_ON_MATCH,
    is_channel_set_output_on_match = TMR_FLAG_IS_CHANNEL_SET_OUTPUT_ON_MATCH,
    is_channel_clear_output_on_match =
        TMR_FLAG_IS_CHANNEL_CLEAR_OUTPUT_ON_MATCH,
    is_channel_toggle_output_on_match =
        TMR_FLAG_IS_CHANNEL_TOGGLE_OUTPUT_ON_MATCH,
    is_channel_pwm_mode = TMR_FLAG_IS_CHANNEL_PWM_MODE,
    set_trigger = TMR_FLAG_SET_TRIGGER,
    is_slave = TMR_FLAG_IS_SLAVE,
    is_master = TMR_FLAG_IS_MASTER,
    is_trigger_reload = TMR_FLAG_IS_TRIGGER_RELOAD,
    is_trigger_reset = TMR_FLAG_IS_TRIGGER_RESET,
    is_trigger_enable = TMR_FLAG_IS_TRIGGER_ENABLE,
    is_trigger_oc0 = TMR_FLAG_IS_TRIGGER_OC0,
    is_trigger_oc1 = TMR_FLAG_IS_TRIGGER_OC1,
    is_trigger_oc2 = TMR_FLAG_IS_TRIGGER_OC2,
    is_trigger_oc3 = TMR_FLAG_IS_TRIGGER_OC3,
  };
};

API_OR_NAMED_FLAGS_OPERATOR(TimerFlags, Flags)

class Timer : public DeviceAccess<Timer>, public TimerFlags {
public:
  class Info {
  public:
    Info() { m_info = {0}; }
    Info(const tmr_info_t &info) { m_info = info; }
    bool is_valid() const { return m_info.o_flags != 0; }
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_flags)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)
  private:
    friend class Timer;
    tmr_info_t m_info;
  };

  class Attributes {
  public:
    Attributes() { var::View(m_attributes.pin_assignment).fill<u8>(0xff); }

    Attributes &set_flags(Flags flags) {
      m_attributes.o_flags = static_cast<u32>(flags);
      return *this;
    }

    Flags flags() const { return Flags(m_attributes.o_flags); }
    u32 o_flags() const { return m_attributes.o_flags; }

    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(Attributes, u32, attributes,
                                             frequency, freq)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, period)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, mcu_channel_t, attributes,
                                  channel)
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(Attributes, mcu_pin_t,
                                             attributes.pin_assignment,
                                             channel0, channel[0])
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(Attributes, mcu_pin_t,
                                             attributes.pin_assignment,
                                             channel1, channel[1])
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(Attributes, mcu_pin_t,
                                             attributes.pin_assignment,
                                             channel2, channel[2])
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(Attributes, mcu_pin_t,
                                             attributes.pin_assignment,
                                             channel3, channel[3])

    tmr_attr_t *attributes() { return &m_attributes; }
    const tmr_attr_t *attributes() const { return &m_attributes; }
  private:
    friend class Timer;
    mutable tmr_attr_t m_attributes = {};
  };

  Timer(const var::StringView device,
        fs::OpenMode open_mode =
            DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Timer() {}
  Timer(const Timer &a) = delete;
  Timer &operator=(const Timer &a) = delete;

  Timer(Timer &&a) { DeviceAccess<Timer>::swap(a); }
  Timer &operator=(Timer &&a) {
    DeviceAccess<Timer>::swap(a);
    return *this;
  }

  Timer &set_attributes() { return ioctl(I_TMR_SETATTR, nullptr); }
  const Timer &set_attributes() const { return ioctl(I_TMR_SETATTR, nullptr); }

  Timer &set_attributes(const Attributes &attributes) {
    return ioctl(I_TMR_SETATTR, &attributes.m_attributes);
  }

  const Timer &set_attributes(const Attributes &attributes) const {
    return ioctl(I_TMR_SETATTR, &attributes.m_attributes);
  }

	Timer &set_value(u32 value) { return ioctl(I_TMR_SET, MCU_INT_CAST(value)); }
  const Timer &set_value(const Attributes &attributes) const {
    return ioctl(I_TMR_SET, &attributes.m_attributes);
  }

  u32 get_value() const {
    u32 value = 0;
    ioctl(I_TMR_GET, &value);
    return value;
  }

  Timer &enable() { return ioctl(I_TMR_ENABLE, nullptr); }
  const Timer &enable() const { return ioctl(I_TMR_ENABLE, nullptr); }

  Timer &disable() { return ioctl(I_TMR_DISABLE, nullptr); }
  const Timer &disable() const { return ioctl(I_TMR_DISABLE, nullptr); }

  Timer &set_channel(const mcu_channel_t &channel) {
    return ioctl(I_TMR_SETCHANNEL, (void *)&channel);
  }
  const Timer &set_channel(const mcu_channel_t &channel) const {
    return ioctl(I_TMR_SETCHANNEL, (void *)&channel);
  }

  u32 get_channel(u32 offset) const {
    mcu_channel_t channel = {offset, 0};
    ioctl(I_TMR_GETCHANNEL, &channel);
    return channel.value;
  }

  Info get_info() const {
    tmr_info_t info;
    ioctl(I_TMR_GETINFO, &info);
    return Info(info);
  }

private:
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::Timer::Attributes &a);
Printer &operator<<(Printer &printer, const hal::Timer::Info &a);
} // namespace printer

#endif /* HALAPI_HAL_TIMER_HPP_ */

// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_ADC_HPP_
#define HALAPI_HAL_ADC_HPP_

#include <sos/dev/adc.h>

#include "Device.hpp"

namespace hal {

struct AdcFlags {
  enum class Flags {
    null = 0,
    adc_loc_is_group = ADC_LOC_IS_GROUP,
    set_converter = ADC_FLAG_SET_CONVERTER,
    is_left_justified = ADC_FLAG_IS_LEFT_JUSTIFIED,
    is_right_justified = ADC_FLAG_IS_RIGHT_JUSTIFIED,
    set_master = ADC_FLAG_SET_MASTER,
    set_slave = ADC_FLAG_SET_SLAVE,
    is_trigger_tmr = ADC_FLAG_IS_TRIGGER_TMR,
    is_trigger_eint = ADC_FLAG_IS_TRIGGER_EINT,
    set_channels = ADC_FLAG_SET_CHANNELS,
    is_scan_mode = ADC_FLAG_IS_SCAN_MODE,
    is_trigger_eint_edge_rising = ADC_FLAG_IS_TRIGGER_EINT_EDGE_RISING,
    is_trigger_eint_edge_falling = ADC_FLAG_IS_TRIGGER_EINT_EDGE_FALLING,
    is_group = ADC_FLAG_IS_GROUP,
    is_continous_conversion = ADC_FLAG_IS_CONTINOUS_CONVERSION
  };
};

API_OR_NAMED_FLAGS_OPERATOR(AdcFlags, Flags)

class Adc : public DeviceAccess<Adc>, public AdcFlags {
public:
  class Info {
  public:
    Info() : m_info{} {}
    explicit Info(const adc_info_t &info) : m_info(info) {}
    API_NO_DISCARD bool is_valid() const { return m_info.o_flags != 0; }
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_flags)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, maximum)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, reference_mv)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, internal_vref_channel)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(
      Info,
      u32,
      info,
      internal_temperature_channel)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, internal_vbat_channel)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, bytes_per_sample)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, resolution)
  private:
    friend class Adc;
    adc_info_t m_info;
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

    class SetGroupChannel {
      API_AF(SetGroupChannel, u16, channel, 0);
      API_AF(SetGroupChannel, u32, rank, 0);
      API_AF(SetGroupChannel, u32, sampling_time, 15);
      API_AF(SetGroupChannel, mcu_pin_t, pin, {0xff});
    };

    Attributes &set_group_channel(const SetGroupChannel &options) {
      set_flags(Flags::set_channels | Flags::is_group);
      set_channel(options.channel());
      set_rank(options.rank());
      set_sampling_time(options.sampling_time());
      if (options.pin().port != 0xff) {
        set_channel0(options.pin());
      }
      return *this;
    }

    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u8, attributes, width)
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      u32,
      attributes,
      frequency,
      freq)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, mcu_pin_t, attributes, trigger)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u8, attributes, channel_count)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u16, attributes, channel)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, rank)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, sampling_time)

    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      channel0,
      channel[0])
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      channel1,
      channel[1])
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      channel2,
      channel[2])
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      channel3,
      channel[3])

    adc_attr_t *attributes() { return &m_attributes; }
    const adc_attr_t *attributes() const { return &m_attributes; }

  private:
    friend class Adc;
    mutable adc_attr_t m_attributes;
  };

  Adc() = default;
  Adc(
    const var::StringView device,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Adc &set_attributes() { return ioctl(I_ADC_SETATTR); }
  const Adc &set_attributes() const { return ioctl(I_ADC_SETATTR); }

  Adc &set_attributes(const Attributes &attributes) {
    return ioctl(I_ADC_SETATTR, &attributes.m_attributes);
  }

  const Adc &set_attributes(const Attributes &attributes) const {
    return ioctl(I_ADC_SETATTR, &attributes.m_attributes);
  }

  API_NO_DISCARD Info get_info() {
    adc_info_t info;
    ioctl(I_ADC_GETINFO, &info);
    return Info(info);
  }

private:
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::Adc::Attributes &a);
Printer &operator<<(Printer &printer, const hal::Adc::Info &a);
} // namespace printer

#endif /* HALAPI_HAL_ADC_HPP_ */

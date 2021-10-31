// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_I2S_HPP_
#define HALAPI_HAL_I2S_HPP_

#include <sos/dev/i2s.h>

#include "Device.hpp"

namespace hal {

class I2SFlags {
public:
  enum class Flags {
    null = 0,
    is_width_8 = I2S_FLAG_IS_WIDTH_8,
    is_width_16 = I2S_FLAG_IS_WIDTH_16,
    is_width_24 = I2S_FLAG_IS_WIDTH_24,
    is_width_32 = I2S_FLAG_IS_WIDTH_32,
    is_mono = I2S_FLAG_IS_MONO,
    is_stereo = I2S_FLAG_IS_STEREO,
    set_master = I2S_FLAG_SET_MASTER,
    set_slave = I2S_FLAG_SET_SLAVE,
    is_transmitter = I2S_FLAG_IS_TRANSMITTER,
    is_receiver = I2S_FLAG_IS_RECEIVER,
    is_format_msb = I2S_FLAG_IS_FORMAT_MSB,
    is_format_lsb = I2S_FLAG_IS_FORMAT_LSB,
    is_mck_enabled = I2S_FLAG_IS_MCK_ENABLED,
    is_format_pcm_short = I2S_FLAG_IS_FORMAT_PCM_SHORT,
    is_format_pcm_long = I2S_FLAG_IS_FORMAT_PCM_LONG,
    is_width_16_extended = I2S_FLAG_IS_WIDTH_16_EXTENDED,
    is_outputdrive_disable = I2S_FLAG_IS_OUTPUTDRIVE_DISABLE,
    is_fifothreshold_empty = I2S_FLAG_IS_FIFOTHRESHOLD_EMPTY,
    is_fifothreshold_1qf = I2S_FLAG_IS_FIFOTHRESHOLD_1QF,
    is_fifothreshold_hf = I2S_FLAG_IS_FIFOTHRESHOLD_HF,
    is_fifothreshold_3qf = I2S_FLAG_IS_FIFOTHRESHOLD_3QF,
    is_fifothreshold_full = I2S_FLAG_IS_FIFOTHRESHOLD_FULL,
    is_ulaw_1cpl_companding = I2S_FLAG_IS_ULAW_1CPL_COMPANDING,
    is_alaw_1cpl_companding = I2S_FLAG_IS_ALAW_1CPL_COMPANDING,
    is_ulaw_2cpl_companding = I2S_FLAG_IS_ULAW_2CPL_COMPANDING,
    is_alaw_2cpl_companding = I2S_FLAG_IS_ALAW_2CPL_COMPANDING,
    enable = I2S_FLAG_ENABLE,
    is_synchronous = I2S_FLAG_IS_SYNCHRONOUS,
    is_synchronous_ext_sai1 = I2S_FLAG_IS_SYNCHRONOUS_EXT_SAI1,
    is_synchronous_ext_sai2 = I2S_FLAG_IS_SYNCHRONOUS_EXT_SAI2,
    set_slot = I2S_FLAG_SET_SLOT
  };
};

API_OR_NAMED_FLAGS_OPERATOR(I2SFlags, Flags)

class I2S : public DeviceAccess<I2S>, public I2SFlags {
public:
  class Attributes {
  public:
    Attributes() {
      set_frequency(100000);
      var::View(m_attributes.pin_assignment).fill<u8>(0xff);
    }

    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      ws)

    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      sck)

    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      sdout)

    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      sdin)

    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      mck)

    Attributes &set_flags(Flags value) {
      m_attributes.o_flags = static_cast<u32>(value);
      return *this;
    }

    API_NO_DISCARD Flags flags() const { return Flags(m_attributes.o_flags); }

    API_NO_DISCARD u32 o_flags() const { return m_attributes.o_flags; }

    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      u32,
      attributes,
      frequency,
      freq)

    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      u32,
      attributes,
      master_multiplier,
      mck_mult)

    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, slot)

    i2s_attr_t *attributes() { return &m_attributes; }
    API_NO_DISCARD const i2s_attr_t *attributes() const {
      return &m_attributes;
    }

  private:
    friend class I2S;
    i2s_attr_t m_attributes{};
  };

  class Info {
  public:
    Info() = default;
    explicit Info(const i2s_info_t &info) : m_info(info) {}
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(Info, u32, info, frequency, freq)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_flags)
    API_NO_DISCARD Flags flags() const { return Flags(m_info.o_flags); }

  private:
    friend class I2S;
    i2s_info_t m_info{};
  };

  explicit I2S(
    const var::StringView device,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  I2S() = default;
  I2S(const I2S &a) = delete;
  I2S &operator=(const I2S &a) = delete;

  I2S(I2S &&a) noexcept { swap(a); }
  I2S &operator=(I2S &&a) noexcept {
    swap(a);
    return *this;
  }

  const I2S &set_attributes() const { return ioctl(I_I2S_SETATTR, nullptr); }
  I2S &set_attributes() { return API_CONST_CAST_SELF(I2S, set_attributes); }

  const I2S &set_attributes(const Attributes &attributes) const {
    return ioctl(I_I2S_SETATTR, (void *)&attributes.m_attributes);
  }

  I2S &set_attributes(const Attributes &attributes) {
    return API_CONST_CAST_SELF(I2S, set_attributes, attributes);
  }

  API_NO_DISCARD Info get_info() {
    i2s_info_t info;
    ioctl(I_I2S_GETINFO, &info);
    return Info(info);
  }

  const I2S &mute() const { return ioctl(I_I2S_MUTE, nullptr); }

  I2S &mute() { return ioctl(I_I2S_MUTE, nullptr); }

  const I2S &unmute() const { return ioctl(I_I2S_UNMUTE, nullptr); }

  I2S &unmute() { return ioctl(I_I2S_UNMUTE, nullptr); }

private:
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::I2S::Attributes &a);
Printer &operator<<(Printer &printer, const hal::I2S::Info &a);
} // namespace printer

#endif /* HALAPI_HAL_I2S_HPP_ */

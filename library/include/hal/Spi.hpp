// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_SPI_HPP_
#define HALAPI_HAL_SPI_HPP_

#include <sos/dev/spi.h>

#include "Device.hpp"

namespace hal {

struct SpiFlags {
  enum class Flags {
    null = 0,
    is_format_spi = SPI_FLAG_IS_FORMAT_SPI,
    is_format_ti = SPI_FLAG_IS_FORMAT_TI,
    is_format_microwire = SPI_FLAG_IS_FORMAT_MICROWIRE,
    is_mode0 = SPI_FLAG_IS_MODE0,
    is_mode1 = SPI_FLAG_IS_MODE1,
    is_mode2 = SPI_FLAG_IS_MODE2,
    is_mode3 = SPI_FLAG_IS_MODE3,
    set_master = SPI_FLAG_SET_MASTER,
    set_slave = SPI_FLAG_SET_SLAVE,
    set_full_duplex = SPI_FLAG_SET_FULL_DUPLEX,
    set_half_duplex = SPI_FLAG_SET_HALF_DUPLEX,
    assert_cs = SPI_FLAG_ASSERT_CS,
    deassert_cs = SPI_FLAG_DEASSERT_CS,
  };
};

API_OR_NAMED_FLAGS_OPERATOR(SpiFlags, Flags)

class Spi : public DeviceAccess<Spi>, public SpiFlags {
public:
  class Info {
  public:
    Info() = default;
    explicit Info(const spi_info_t &info) { m_info = info; }
    API_NO_DISCARD bool is_valid() const { return m_info.o_flags != 0; }
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_flags)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)
  private:
    friend class Spi;
    spi_info_t m_info{};
  };

  class Attributes {
  public:
    Attributes() {
      set_flags(
        Flags::set_master | Flags::is_format_spi | Flags::is_mode0
        | Flags::set_half_duplex);
      set_frequency(1000000);
      set_width(8);
      var::View(m_attributes.pin_assignment).fill<u8>(0xff);
    }

    Attributes &set_flags(Flags flags) {
      m_attributes.o_flags = static_cast<u32>(flags);
      return *this;
    }

    API_NO_DISCARD Flags flags() const { return Flags(m_attributes.o_flags); }
    API_NO_DISCARD u32 o_flags() const { return m_attributes.o_flags; }

    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u8, attributes, width)
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      u32,
      attributes,
      frequency,
      freq)
    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      miso)
    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      mosi)
    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      sck)
    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      cs)

    spi_attr_t *attributes() { return &m_attributes; }
    API_NO_DISCARD const spi_attr_t *attributes() const {
      return &m_attributes;
    }

  private:
    friend class Spi;
    mutable spi_attr_t m_attributes{};
  };

  Spi(
    const var::StringView device,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Spi() = default;
  Spi(const Spi &a) = delete;
  Spi &operator=(const Spi &a) = delete;

  Spi(Spi &&a) noexcept { DeviceAccess<Spi>::swap(a); }
  Spi &operator=(Spi &&a) noexcept {
    DeviceAccess<Spi>::swap(a);
    return *this;
  }

  Spi &set_attributes() { return ioctl(I_SPI_SETATTR); }
  const Spi &set_attributes() const { return ioctl(I_SPI_SETATTR); }

  Spi &set_attributes(const Attributes &attributes) {
    return ioctl(I_SPI_SETATTR, &attributes.m_attributes);
  }

  const Spi &set_attributes(const Attributes &attributes) const {
    return ioctl(I_SPI_SETATTR, &attributes.m_attributes);
  }

  Spi &assert_cs() {
    return set_attributes(Attributes().set_flags(Flags::assert_cs));
  }

  const Spi &assert_cs() const {
    return set_attributes(Attributes().set_flags(Flags::assert_cs));
  }

  Spi &deassert_cs() {
    return set_attributes(Attributes().set_flags(Flags::deassert_cs));
  }

  const Spi &deassert_cs() const {
    return set_attributes(Attributes().set_flags(Flags::deassert_cs));
  }

  API_NO_DISCARD Info get_info() {
    spi_info_t info;
    ioctl(I_SPI_GETINFO, &info);
    return Info(info);
  }

  Spi &swap(u32 value) { return ioctl(I_SPI_SWAP, MCU_INT_CAST(value)); }

private:
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::Spi::Attributes &a);
Printer &operator<<(Printer &printer, const hal::Spi::Info &a);
} // namespace printer

#endif /* HALAPI_HAL_SPI_HPP_ */

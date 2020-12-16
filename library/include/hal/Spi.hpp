/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.

#ifndef HALAPI_HAL_SPI_HPP_
#define HALAPI_HAL_SPI_HPP_

#include <sos/dev/spi.h>

#include "Device.hpp"
#include "PinAssignment.hpp"

namespace hal {

struct SpiFlags {
  enum class Flags {
    null = 0,
    is_format_spi /*! See \ref SPI_FLAG_IS_FORMAT_SPI */
    = SPI_FLAG_IS_FORMAT_SPI,
    is_format_ti /*! See \ref SPI_FLAG_IS_FORMAT_TI */
    = SPI_FLAG_IS_FORMAT_TI,
    is_format_microwire /*! See \ref SPI_FLAG_IS_FORMAT_MICROWIRE */
    = SPI_FLAG_IS_FORMAT_MICROWIRE,
    is_mode0 /*! See \ref SPI_FLAG_IS_MODE0 */ = SPI_FLAG_IS_MODE0,
    is_mode1 /*! See \ref SPI_FLAG_IS_MODE1 */ = SPI_FLAG_IS_MODE1,
    is_mode2 /*! See \ref SPI_FLAG_IS_MODE2 */ = SPI_FLAG_IS_MODE2,
    is_mode3 /*! See \ref SPI_FLAG_IS_MODE3 */ = SPI_FLAG_IS_MODE3,
    set_master /*! See \ref SPI_FLAG_SET_MASTER */ = SPI_FLAG_SET_MASTER,
    set_slave /*! See \ref SPI_FLAG_SET_SLAVE */ = SPI_FLAG_SET_SLAVE,
    is_full_duplex /*! See \ref SPI_FLAG_IS_FULL_DUPLEX */
    = SPI_FLAG_IS_FULL_DUPLEX,
    set_full_duplex /*! See \ref SPI_FLAG_IS_FULL_DUPLEX */
    = SPI_FLAG_SET_FULL_DUPLEX,
    is_half_duplex /*! See \ref SPI_FLAG_IS_HALF_DUPLEX */
    = SPI_FLAG_IS_HALF_DUPLEX,
    set_half_duplex /*! See \ref SPI_FLAG_IS_FULL_DUPLEX */
    = SPI_FLAG_SET_HALF_DUPLEX,
  };
};

API_OR_NAMED_FLAGS_OPERATOR(SpiFlags, Flags)

/*! \brief SPI Class
 * \details This class gives access to a SPI port.
 *
 * \code
 * #include <hal.hpp>
 *
 * int main(int argc, char * argv[]){
 *   char buffer[16];
 *   Spi spi(0);           //access to SPI port 0
 *   spi.init();           //init SPI with default settings from BSP
 *   spi.read(buffer, 16); //read 16 bytes from the SPI
 *   spi.close();          //close the SPI (power it off)
 *   return 0;
 * }
 * \endcode
 */
class Spi : public DeviceAccess<Spi>, public SpiFlags {
public:
  class Info {
  public:
    Info() { m_info = {0}; }
    Info(const spi_info_t &info) { m_info = info; }
    bool is_valid() const { return m_info.o_flags != 0; }
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_flags)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)
  private:
    friend class Spi;
    spi_info_t m_info;
  };

  class Attributes {
  public:
    Attributes() {
      set_flags(
        Flags::set_master | Flags::is_format_spi | Flags::is_mode0
        | Flags::is_half_duplex);
      set_frequency(1000000);
      set_width(8);
      var::View(m_attributes.pin_assignment).fill<u8>(0xff);
    }

    Attributes &set_flags(Flags flags) {
      m_attributes.o_flags = static_cast<u32>(flags);
      return *this;
    }

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

  private:
    friend class Spi;
    mutable spi_attr_t m_attributes;
  };

  Spi(const var::StringView device,
      fs::OpenMode open_mode =
          DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Spi() {}
  Spi(const Spi &a) = delete;
  Spi &operator=(const Spi &a) = delete;

  Spi(Spi &&a) { DeviceAccess<Spi>::swap(a); }
  Spi &operator=(Spi &&a) {
    DeviceAccess<Spi>::swap(a);
    return *this;
  }

  Spi &set_attributes(const Attributes &attributes) {
    return ioctl(I_SPI_SETATTR, &attributes.m_attributes);
  }

  const Spi &set_attributes(const Attributes &attributes) const {
    return ioctl(I_SPI_SETATTR, &attributes.m_attributes);
  }

  Info get_info() {
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

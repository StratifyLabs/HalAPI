/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.
#ifndef HALAPI_HAL_DRIVE_HPP_
#define HALAPI_HAL_DRIVE_HPP_

#include <sos/dev/drive.h>

#include "Device.hpp"
#include "chrono/DateTime.hpp"

namespace hal {

struct DriveFlags {
  enum class Flags {
    protect = DRIVE_FLAG_PROTECT,
    unprotect = DRIVE_FLAG_UNPROTECT,
    erase_blocks = DRIVE_FLAG_ERASE_BLOCKS,
    erase_sectors = DRIVE_FLAG_ERASE_SECTORS,
    erase_device = DRIVE_FLAG_ERASE_DEVICE,
    powerdown = DRIVE_FLAG_POWERDOWN,
    powerup = DRIVE_FLAG_POWERUP,
    initialize = DRIVE_FLAG_INIT,
    reset = DRIVE_FLAG_RESET
  };
};

API_OR_NAMED_FLAGS_OPERATOR(DriveFlags, Flags)

class Drive : public DeviceAccess<Drive>, public DriveFlags {
public:
  class Info {
  public:
    Info() { m_info = {0}; }
    explicit Info(const drive_info_t &info) : m_info(info) {}

    bool is_valid() const { return m_info.num_write_blocks != 0; }

    u64 size() const {
      return m_info.num_write_blocks * m_info.write_block_size;
    }

    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_flags)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)

    Flags flags() const { return Flags(m_info.o_flags); }
    Flags events() const { return Flags(m_info.o_events); }

    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u16, info, addressable_size)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u16, info, write_block_size)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, num_write_blocks)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, erase_block_size)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, bitrate)

    u32 frequency() const { return m_info.bitrate; }

    chrono::MicroTime erase_device_time() const {
      return chrono::MicroTime(m_info.erase_device_time);
    }

    chrono::MicroTime erase_block_time() const {
      return chrono::MicroTime(m_info.erase_block_time);
    }

    u32 page_program_size() const { return m_info.page_program_size; }

  private:
    drive_info_t m_info;
  };

  class Attributes {
  public:
    Attributes() { m_attributes = {0}; }

    Attributes(const drive_attr_t &attr) : m_attributes(attr) {}

    Attributes &set_flags(Flags flags) {
      m_attributes.o_flags = static_cast<u32>(flags);
      return *this;
    }

    Flags flags() const { return Flags(o_flags()); }

    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, o_flags)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, start)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, end)

  private:
    friend class Drive;
    mutable drive_attr_t m_attributes;
  };

  Drive(const var::StringView device,
        fs::OpenMode open_mode =
            DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Drive() {}
  Drive(const Drive &a) = delete;
  Drive &operator=(const Drive &a) = delete;

  Drive(Drive &&a) { DeviceAccess<Drive>::swap(a); }
  Drive &operator=(Drive &&a) {
    DeviceAccess<Drive>::swap(a);
    return *this;
  }

  Drive &set_attributes() { return ioctl(I_DRIVE_SETATTR, nullptr); }
  const Drive &set_attributes() const {
    return ioctl(I_DRIVE_SETATTR, nullptr);
  }

  Drive &set_attributes(const Attributes &attributes) {
    return ioctl(I_DRIVE_SETATTR, &attributes.m_attributes);
  }

  const Drive &set_attributes(const Attributes &attributes) const {
    return ioctl(I_DRIVE_SETATTR, &attributes.m_attributes);
  }

  const Drive &erase_device() const;
  Drive &erase_device() { return API_CONST_CAST_SELF(Drive, erase_device); }

  bool is_busy() const {
    return ioctl(I_DRIVE_ISBUSY, nullptr).return_value() > 0;
  }

  Info get_info() const {
    drive_info_t info;
    ioctl(I_DRIVE_GETINFO, &info);
    return Info(info);
  }

private:
  int set_attributes(const drive_attr_t &attributes) const;
};

} // namespace hal

namespace printer {
class Printer;
Printer &operator<<(Printer &printer, const hal::Drive::Info &a);
Printer &operator<<(Printer &printer, const hal::Drive::Attributes &a);
} // namespace printer

#endif // HALAPI_HAL_DRIVE_HPP_

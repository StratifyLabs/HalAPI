// Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
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
    Info() = default;
    explicit Info(const drive_info_t &info) : m_info(info) {}

    API_NO_DISCARD bool is_valid() const {
      return m_info.num_write_blocks != 0;
    }

    API_NO_DISCARD u64 size() const {
      return m_info.num_write_blocks * m_info.write_block_size;
    }

    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_flags)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)

    API_NO_DISCARD Flags flags() const { return Flags(m_info.o_flags); }
    API_NO_DISCARD Flags events() const { return Flags(m_info.o_events); }

    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u16, info, addressable_size)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u16, info, write_block_size)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, num_write_blocks)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, erase_block_size)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, bitrate)

    API_NO_DISCARD u32 frequency() const { return m_info.bitrate; }

    API_NO_DISCARD chrono::MicroTime erase_device_time() const {
      return chrono::MicroTime(m_info.erase_device_time);
    }

    API_NO_DISCARD chrono::MicroTime erase_block_time() const {
      return chrono::MicroTime(m_info.erase_block_time);
    }

    API_NO_DISCARD u32 page_program_size() const {
      return m_info.page_program_size;
    }

  private:
    drive_info_t m_info{};
  };

  class Attributes {
  public:
    Attributes() = default;

    Attributes(const drive_attr_t &attr) : m_attributes(attr) {}

    Attributes &set_flags(Flags flags) {
      m_attributes.o_flags = static_cast<u32>(flags);
      return *this;
    }

    API_NO_DISCARD Flags flags() const { return Flags(o_flags()); }

    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, o_flags)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, start)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, end)

  private:
    friend class Drive;
    mutable drive_attr_t m_attributes{};
  };

  Drive(
    const var::StringView device,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Drive() = default;
  Drive &set_attributes() { return ioctl(I_DRIVE_SETATTR); }
  const Drive &set_attributes() const {
    return ioctl(I_DRIVE_SETATTR);
  }

  Drive &set_attributes(const Attributes &attributes) {
    return ioctl(I_DRIVE_SETATTR, &attributes.m_attributes);
  }

  const Drive &set_attributes(const Attributes &attributes) const {
    return ioctl(I_DRIVE_SETATTR, &attributes.m_attributes);
  }

  const Drive &erase_device() const;
  Drive &erase_device() { return API_CONST_CAST_SELF(Drive, erase_device); }

  const Drive &erase_blocks(u32 start_address, u32 end_address) const;
  Drive &erase_blocks(u32 start_address, u32 end_address) {
    return API_CONST_CAST_SELF(Drive, erase_blocks, start_address, end_address);
  }

  const Drive &unprotect() const {
    return set_attributes(Attributes().set_flags(Flags::unprotect));
  }
  Drive &unprotect() { return API_CONST_CAST_SELF(Drive, unprotect); }

  const Drive &reset() const {
    return set_attributes(Attributes().set_flags(Flags::reset));
  }
  Drive &reset() { return API_CONST_CAST_SELF(Drive, reset); }

  const Drive &protect() const {
    return set_attributes(Attributes().set_flags(Flags::protect));
  }
  Drive &protect() { return API_CONST_CAST_SELF(Drive, protect); }

  API_NO_DISCARD bool is_busy() const {
    return ioctl(I_DRIVE_ISBUSY).return_value() > 0;
  }

  API_NO_DISCARD Info get_info() const {
    drive_info_t info;
    ioctl(I_DRIVE_GETINFO, &info);
    return Info(info);
  }
};

} // namespace hal

namespace printer {
class Printer;
Printer &operator<<(Printer &printer, const hal::Drive::Info &a);
Printer &operator<<(Printer &printer, const hal::Drive::Attributes &a);
} // namespace printer

#endif // HALAPI_HAL_DRIVE_HPP_

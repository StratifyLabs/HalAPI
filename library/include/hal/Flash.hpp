// Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
// LICENSE.md for rights.

#ifndef HALAPI_HAL_FLASH_HPP_
#define HALAPI_HAL_FLASH_HPP_

#include <sos/dev/flash.h>

#include "Device.hpp"

namespace hal {

struct FlashFlags {
  enum class Flags { null = 0 };
};

API_OR_NAMED_FLAGS_OPERATOR(FlashFlags, Flags)

class Flash : public DeviceAccess<Flash>, public FlashFlags {
public:
  class Info {
  public:
    Info() = default;
    explicit Info(const flash_info_t &info) : m_info(info) {}

    API_NO_DISCARD bool is_valid() const { return m_info.o_events != 0; }

    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)

    API_NO_DISCARD Flags flags() const { return Flags(m_info.o_flags); }
    API_NO_DISCARD Flags events() const { return Flags(m_info.o_events); }

  private:
    flash_info_t m_info{};
  };

  class PageInfo {
  public:
    PageInfo() = default;
    explicit PageInfo(const flash_pageinfo_t &page_info)
      : m_page_info(page_info) {}

    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, page_info, page)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, page_info, size)

    API_NO_DISCARD u32 address() const { return m_page_info.addr; }

    API_NO_DISCARD bool is_valid() const { return size() != 0; }

  private:
    flash_pageinfo_t m_page_info{};
  };

  Flash(
    const var::StringView device,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Flash() = default;

  API_NO_DISCARD int get_version() const {
    return ioctl(I_FLASH_GETVERSION).return_value();
  }

  const Flash &erase_device() const;
  Flash &erase_device() { return API_CONST_CAST_SELF(Flash, erase_device); }

  const Flash &write_page(u32 address, var::View data) const;
  Flash &write_page(u32 address, var::View data) {
    return API_CONST_CAST_SELF(Flash, write_page, address, data);
  }

  const Flash &erase_page(u32 page_number) const;
  Flash &erase_page(u32 page_number) {
    return API_CONST_CAST_SELF(Flash, erase_page, page_number);
  }

  const Flash &erase_address(u32 address) const;
  Flash &erase_address(u32 address) {
    return API_CONST_CAST_SELF(Flash, erase_address, address);
  }

  API_NO_DISCARD Info get_info() const {
    flash_info_t info;
    ioctl(I_FLASH_GETINFO, &info);
    return Info(info);
  }

  API_NO_DISCARD var::Vector<PageInfo> get_page_info() const;

  API_NO_DISCARD u32 get_size() const;

  API_NO_DISCARD PageInfo get_page_info(u32 page) const;
  API_NO_DISCARD u32 get_page_from_address(u32 address) const;
};

} // namespace hal

namespace printer {
class Printer;
Printer &operator<<(Printer &printer, const hal::Flash::PageInfo &a);
} // namespace printer

#endif // HALAPI_HAL_FLASH_HPP_

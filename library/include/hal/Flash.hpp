// Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
// LICENSE.md for rights.

#ifndef HALAPI_HAL_FLASH_HPP_
#define HALAPI_HAL_FLASH_HPP_

#include <sos/dev/flash.h>

#include "Device.hpp"


namespace hal {

struct FlashFlags {
  enum class Flags {
    null = 0
  };
};

API_OR_NAMED_FLAGS_OPERATOR(FlashFlags, Flags)

class Flash : public DeviceAccess<Flash>, public FlashFlags
{
public:
  class Info {
  public:
    Info() : m_info{} {}
    explicit Info(const flash_info_t &info) : m_info(info) {}

    bool is_valid() const { return m_info.o_events != 0; }

    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)

    Flags flags() const { return Flags(m_info.o_flags); }
    Flags events() const { return Flags(m_info.o_events); }

  private:
    flash_info_t m_info;
  };

  class PageInfo {
  public:

    PageInfo() : m_page_info{} {}
    explicit PageInfo(const flash_pageinfo_t & page_info) : m_page_info(page_info){}

    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, page_info, page)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, page_info, size)

    u32 address() const {
      return m_page_info.addr;
    }

    bool is_valid() const {
      return size() != 0;
    }

  private:
    flash_pageinfo_t m_page_info;
  };

  Flash(const var::StringView device,
        fs::OpenMode open_mode =
          DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Flash() {}
  Flash(const Flash &a) = delete;
  Flash &operator=(const Flash &a) = delete;

  Flash(Flash &&a) { DeviceAccess<Flash>::swap(a); }
  Flash &operator=(Flash &&a) {
    DeviceAccess<Flash>::swap(a);
    return *this;
  }

  int get_version() const {
    return ioctl(I_FLASH_GETVERSION, nullptr).return_value();
  }

  const Flash &erase_device() const;
  Flash &erase_device() { return API_CONST_CAST_SELF(Flash, erase_device); }


  const Flash &write_page(u32 address, var::View data) const;
  Flash &write_page(u32 address, var::View data){
    return API_CONST_CAST_SELF(Flash, write_page, address, data);
  }

  const Flash &erase_page(u32 page_number) const;
  Flash &erase_page(u32 page_number){
    return API_CONST_CAST_SELF(Flash, erase_page, page_number);
  }

  const Flash &erase_address(u32 address) const;
  Flash &erase_address(u32 address){
    return API_CONST_CAST_SELF(Flash, erase_address, address);
  }

  Info get_info() const {
    flash_info_t info;
    ioctl(I_FLASH_GETINFO, &info);
    return Info(info);
  }

  var::Vector<PageInfo> get_page_info() const;

  u32 get_size() const;

  PageInfo get_page_info(u32 page) const;
  u32 get_page_from_address(u32 address) const;


};

}

namespace printer {
class Printer;
Printer &operator<<(Printer &printer, const hal::Flash::PageInfo &a);
} // namespace printer

#endif // HALAPI_HAL_FLASH_HPP_

#include "hal/Flash.hpp"

#include <printer/Printer.hpp>

printer::Printer &
printer::operator<<(printer::Printer &printer, const hal::Flash::PageInfo &a) {
  return printer.key("page", var::NumberString(a.page()))
    .key("address", var::NumberString(a.address(), "0x%lX"))
    .key("size", var::NumberString(a.size()));
}

using namespace hal;

Flash::PageInfo Flash::get_page_info(u32 page) const {
  flash_pageinfo_t page_info = {.page = page};
  ioctl(I_FLASH_GET_PAGEINFO, &page_info);
  return PageInfo(page_info);
}

var::Vector<Flash::PageInfo> Flash::get_page_info() const {
  var::Vector<Flash::PageInfo> result;
  u32 count = 0;

  // first count the pages
  bool is_done;
  do {
    api::ErrorScope es;
    const auto info = get_page_info(count);
    is_done = !info.is_valid();
    if (!is_done) {
      count++;
    }
  } while (!is_done);

  if (count == 0) {
    return result;
  }

  result.reserve(count);

  // now grab the info
  count = 0;
  do {
    api::ErrorScope es;
    const auto info = get_page_info(count);
    is_done = !info.is_valid();
    if (!is_done) {
      count++;
      result.push_back(info);
    }
  } while (!is_done);

  return result;
}

u32 Flash::get_page_from_address(u32 address) const {
  return ioctl(I_FLASH_GETPAGE, MCU_INT_CAST(address)).return_value();
}

u32 Flash::get_size() const {
  return ioctl(I_FLASH_GETSIZE, nullptr).return_value();
}

const Flash &Flash::write_page(u32 address, var::View data) const {
  flash_writepage_t write_page = {.addr = address};
  API_ASSERT(data.size() <= sizeof(write_page.buf));
  write_page.nbyte = data.size();
  var::View(write_page.buf).copy(data);
  ioctl(I_FLASH_WRITEPAGE, &write_page);
  return *this;
}

const Flash &Flash::erase_page(u32 page_number) const {
  ioctl(I_FLASH_ERASE_PAGE, MCU_INT_CAST(page_number));
  return *this;
}

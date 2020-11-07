/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.

#ifndef HALAPI_HAL_UART_HPP_
#define HALAPI_HAL_UART_HPP_

#include <sos/dev/uart.h>

#include "Device.hpp"
#include "printer/Printer.hpp"

namespace hal {

class UartFlags {
public:
  enum class Flags {
    set_line_coding_default /*! See \ref UART_FLAG_SET_LINE_CODING_DEFAULT
                             */
    = UART_FLAG_SET_LINE_CODING_DEFAULT,
    set_line_coding /*! See \ref UART_FLAG_SET_LINE_CODING */
    = UART_FLAG_SET_LINE_CODING,
    is_stop1 /*! See \ref UART_FLAG_IS_STOP1 */ = UART_FLAG_IS_STOP1,
    is_stop2 /*! See \ref UART_FLAG_IS_STOP2 */ = UART_FLAG_IS_STOP2,
    is_stop0_5 /*! See \ref UART_FLAG_IS_STOP0_5 */ = UART_FLAG_IS_STOP0_5,
    is_stop1_5 /*! See \ref UART_FLAG_IS_STOP1_5 */ = UART_FLAG_IS_STOP1_5,
    is_parity_none /*! See \ref UART_FLAG_IS_PARITY_NONE */
    = UART_FLAG_IS_PARITY_NONE,
    is_parity_odd /*! See \ref UART_FLAG_IS_PARITY_ODD */
    = UART_FLAG_IS_PARITY_ODD,
    is_parity_even /*! See \ref UART_FLAG_IS_PARITY_EVEN */
    = UART_FLAG_IS_PARITY_EVEN,
  };
};

API_OR_NAMED_FLAGS_OPERATOR(UartFlags, Flags)

class Uart : public DeviceAccess<Uart>, public UartFlags {
public:
  class Attributes {
  public:
    Attributes() {
      set_flags(Flags::set_line_coding_default);
      set_frequency(115200);
      set_width(8);
    }

    bool is_valid() const { return frequency() != 0; }

    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      tx)
    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      rx)
    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      cts)
    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      rts)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, width)
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      u32,
      attributes,
      frequency,
      freq)

    Attributes &set_flags(Flags value) {
      m_attributes.o_flags = static_cast<u32>(value);
      return *this;
    }

  private:
    friend class Uart;
    uart_attr_t m_attributes;
  };

  class Info {
  public:
    Info() { m_info = {0}; }
    Info(const uart_info_t &info) { m_info = info; }

    bool is_valid() const { return m_info.o_flags != 0; }

    bool is_rx_fifo() const { return m_info.o_flags & UART_FLAG_IS_RX_FIFO; }

    u32 size() const { return m_info.size; }
    u32 size_ready() const { return m_info.size_ready; }

  private:
    friend class Uart;
    uart_info_t m_info;
  };

  Uart(const var::StringView device FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess(device, DEVICE_OPEN_MODE FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  int get_version() const {
    return ioctl(I_UART_GETVERSION, nullptr).return_value();
  }

  Uart &set_attributes(const Attributes &attr) {
    return ioctl(I_UART_SETATTR, (void *)&attr.m_attributes);
  }

  Uart &put(char c) { return ioctl(I_UART_PUT, &c); }

  Uart &flush() { return ioctl(I_UART_FLUSH, nullptr); }

  char get() const {
    char c;
    ioctl(I_UART_GET, &c);
    return c;
  }

  Info get_info() const {
    uart_info_t info;
    ioctl(I_UART_GETINFO, &info);
    return Info(info);
  }

private:
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::Uart::Attributes &a);
Printer &operator<<(Printer &printer, const hal::Uart::Info &a);
} // namespace printer

#endif /* HALAPI_HAL_UART_HPP_ */

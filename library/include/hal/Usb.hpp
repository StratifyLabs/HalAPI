// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_USB_HPP_
#define HALAPI_HAL_USB_HPP_

#include <sos/dev/usb.h>

#include "Device.hpp"
#include "printer/Printer.hpp"

namespace hal {

class UsbFlags {
public:
  enum class Flags {
    set_unconfigured = USB_FLAG_SET_UNCONFIGURED,
    set_device = USB_FLAG_SET_DEVICE,
    set_host = USB_FLAG_SET_HOST,
    set_otg = USB_FLAG_SET_OTG,
    reset = USB_FLAG_RESET,
    attach = USB_FLAG_ATTACH,
    detach = USB_FLAG_DETACH,
    configure = USB_FLAG_CONFIGURE,
    unconfigure = USB_FLAG_UNCONFIGURE,
    set_address = USB_FLAG_SET_ADDRESS,
    reset_endpoint = USB_FLAG_RESET_ENDPOINT,
    enable_endpoint = USB_FLAG_ENABLE_ENDPOINT,
    disable_endpoint = USB_FLAG_DISABLE_ENDPOINT,
    stall_endpoint = USB_FLAG_STALL_ENDPOINT,
    unstall_endpoint = USB_FLAG_UNSTALL_ENDPOINT,
    configure_endpoint = USB_FLAG_CONFIGURE_ENDPOINT,
    is_sof_enabled = USB_FLAG_IS_SOF_ENABLED,
    is_low_power_mode_enabled = USB_FLAG_IS_LOW_POWER_MODE_ENABLED,
    is_vbus_sensing_enabled = USB_FLAG_IS_VBUS_SENSING_ENABLED,
    is_high_speed = USB_FLAG_IS_HIGH_SPEED,
    is_battery_charging_enabled = USB_FLAG_IS_BATTERY_CHARGING_ENABLED,
    flush_endpoint = USB_FLAG_FLUSH_ENDPOINT
  };
};

API_OR_NAMED_FLAGS_OPERATOR(UsbFlags, Flags)

class Usb : public DeviceAccess<Usb>, public UsbFlags {
public:
  class Attributes {
  public:
    Attributes() { var::View(m_attributes.pin_assignment).fill<u8>(0xff); }

    API_NO_DISCARD bool is_valid() const { return frequency() != 0; }

    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      dp)
    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      dm)
    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      id)
    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      vbus)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, address)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, max_packet_size)
    API_ACCESS_MEMBER_FUNDAMENTAL(Attributes, u32, attributes, type)
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

    API_NO_DISCARD Flags flags() const { return Flags(m_attributes.o_flags); }

    API_NO_DISCARD u32 o_flags() const { return m_attributes.o_flags; }

    usb_attr_t *attributes() { return &m_attributes; }
    API_NO_DISCARD const usb_attr_t *attributes() const {
      return &m_attributes;
    }

  private:
    friend class Usb;
    usb_attr_t m_attributes{};
  };

  class Info {
  public:
    Info() = default;
    explicit Info(const usb_info_t &info) { m_info = info; }

    API_NO_DISCARD Flags flags() const { return Flags(m_info.o_flags); }

    API_NO_DISCARD u32 o_flags() const { return m_info.o_flags; }

    API_NO_DISCARD u32 o_events() const { return m_info.o_events; }

  private:
    friend class Usb;
    usb_info_t m_info{};
  };

  explicit Usb(
    const var::StringView device,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Usb() = default;
  Usb(const Usb &a) = delete;
  Usb &operator=(const Usb &a) = delete;

  Usb(Usb &&a) { swap(a); }
  Usb &operator=(Usb &&a) {
    swap(a);
    return *this;
  }

  API_NO_DISCARD int get_version() const {
    return ioctl(I_USB_GETVERSION, nullptr).return_value();
  }

  Usb &set_attributes() { return ioctl(I_USB_SETATTR, nullptr); }
  const Usb &set_attributes() const { return ioctl(I_USB_SETATTR, nullptr); }

  Usb &set_attributes(const Attributes &attr) {
    return ioctl(I_USB_SETATTR, (void *)&attr.m_attributes);
  }

  const Usb &set_attributes(const Attributes &attr) const {
    return ioctl(I_USB_SETATTR, (void *)&attr.m_attributes);
  }

  const Usb &attach() const {
    return set_attributes(Attributes().set_flags(Flags::attach));
  }

  Usb &attach() { return API_CONST_CAST_SELF(Usb, attach); }

  const Usb &detach() const {
    return set_attributes(Attributes().set_flags(Flags::detach));
  }

  Usb &detach() { return API_CONST_CAST_SELF(Usb, detach); }

  const Usb &reset() const {
    return set_attributes(Attributes().set_flags(Flags::reset));
  }

  Usb &reset() { return API_CONST_CAST_SELF(Usb, reset); }

  const Usb &configure() const {
    return set_attributes(Attributes().set_flags(Flags::configure));
  }

  Usb &configure() { return API_CONST_CAST_SELF(Usb, configure); }

  const Usb &unconfigure() const {
    return set_attributes(Attributes().set_flags(Flags::unconfigure));
  }

  Usb &unconfigure() { return API_CONST_CAST_SELF(Usb, unconfigure); }

  API_NO_DISCARD bool is_connected() const {
    return ioctl(I_USB_ISCONNECTED, nullptr).return_value() != 0;
  }

  API_NO_DISCARD Info get_info() const {
    usb_info_t info;
    ioctl(I_USB_GETINFO, &info);
    return Info(info);
  }

private:
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::Usb::Attributes &a);
Printer &operator<<(Printer &printer, const hal::Usb::Info &a);
} // namespace printer

#endif /* HALAPI_HAL_USB_HPP_ */

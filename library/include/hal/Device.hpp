// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_DEVICE_HPP_
#define HALAPI_HAL_DEVICE_HPP_

#include <sos/Link.hpp>

#include "DeviceSignal.hpp"
#include "fs/Aio.hpp"
#include "fs/File.hpp"

#if defined __link
#define DEVICE_OPEN_MODE (fs::OpenMode::read_write().set_non_blocking())
#else
#define DEVICE_OPEN_MODE fs::OpenMode::read_write()
#endif
namespace hal {

class DeviceObject : public api::ExecutionContext {
public:
  class Channel {
    API_ACCESS_FUNDAMENTAL(Channel, u32, location, 0);
    API_ACCESS_FUNDAMENTAL(Channel, u32, value, 0);
  };

  DeviceObject() {}

  explicit DeviceObject(
    var::StringView path,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : m_file(path, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  API_NO_DISCARD bool is_valid() const { return m_file.is_valid(); }
  DeviceObject(const DeviceObject &a) = delete;
  DeviceObject &operator=(const DeviceObject &a) = delete;

  DeviceObject(DeviceObject &&a) noexcept { swap(a); }
  DeviceObject &operator=(DeviceObject &&a) noexcept {
    swap(a);
    return *this;
  }

#if !defined __link

  class Transfer {
    API_AC(Transfer, var::View, source);
    API_AC(Transfer, var::View, destination);
  };

  API_NO_DISCARD const fs::File &file() const { return m_file; }
  fs::File &file() { return m_file; }

protected:
  void
  set_interrupt_priority(int priority, int request = I_MCU_SETACTION) const;

  void set_signal_action(
    const DeviceSignal &signal,
    const DeviceSignal::CreateAction &options) {
    mcu_action_t action = signal.create_action(options);
    m_file.ioctl(I_MCU_SETACTION, &action);
  }

  void read(fs::Aio &aio) const;
  void write(fs::Aio &aio) const;

  void cancel_read(int channel = 0) const;
  void cancel_write(int channel = 0) const;
  void cancel(int channel, int o_events) const;

  void transfer(const Transfer &options) const;

#endif

protected:
#if defined __link
  sos::Link::File m_file;
#else
  fs::File m_file;
#endif

  void swap(DeviceObject &a) { std::swap(m_file, a.m_file); }
};

template <class Derived>
class DeviceAccess : public DeviceObject, public fs::FileMemberAccess<Derived> {
public:
  DeviceAccess() : fs::FileMemberAccess<Derived>(m_file){};

  DeviceAccess(DeviceAccess &&a) noexcept = default;
  DeviceAccess &operator=(DeviceAccess &&a) noexcept = default;

  explicit DeviceAccess(
    const var::StringView path,
    fs::OpenMode open_mode = fs::OpenMode::read_write()
#if defined __link
                               .set_non_blocking()
#endif
                                 FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceObject(path, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST),
      fs::FileMemberAccess<Derived>(m_file) {
  }

  API_NO_DISCARD int fileno() const { return m_file.fileno(); }

  Derived &&move() { return static_cast<Derived &&>(std::move(*this)); }

#if !defined __link
  const Derived &cancel(int channel, int o_events) const {
    DeviceObject::cancel(channel, o_events);
    return static_cast<const Derived &>(*this);
  }

  Derived &cancel(int channel, int o_events) {
    DeviceObject::cancel(channel, o_events);
    return static_cast<const Derived &>(*this);
  }

  const Derived &cancel_read(int channel = 0) const {
    DeviceObject::cancel_read(channel);
    return static_cast<const Derived &>(*this);
  }

  Derived &cancel_read(int channel = 0) {
    DeviceObject::cancel_read(channel);
    return static_cast<const Derived &>(*this);
  }

  const Derived &cancel_write(int channel = 0) const {
    DeviceObject::cancel_write(channel);
    return static_cast<const Derived &>(*this);
  }

  Derived &cancel_write(int channel = 0) {
    DeviceObject::cancel_write(channel);
    return static_cast<Derived &>(*this);
  }

  const Derived &set_interrupt_priority(int priority) const {
    DeviceObject::set_interrupt_priority(priority);
    return static_cast<Derived &>(*this);
  }

  Derived &set_interrupt_priority(int priority) {
    DeviceObject::set_interrupt_priority(priority);
    return static_cast<Derived &>(*this);
  }

  using fs::FileMemberAccess<Derived>::read;
  using fs::FileMemberAccess<Derived>::write;

  const Derived &read(fs::Aio &aio) const {
    DeviceObject::read(aio);
    return static_cast<const Derived &>(*this);
  }

  Derived &read(fs::Aio &aio) {
    DeviceObject::read(aio);
    return static_cast<Derived &>(*this);
  }

  const Derived &write(fs::Aio &aio) const {
    DeviceObject::write(aio);
    return static_cast<const Derived &>(*this);
  }

  Derived &write(fs::Aio &aio) {
    DeviceObject::write(aio);
    return static_cast<Derived &>(*this);
  }

  const Derived &transfer(const Transfer &options) const {
    DeviceObject::transfer(options);
    return static_cast<const Derived &>(*this);
  }

  Derived &transfer(const Transfer &options) {
    DeviceObject::transfer(options);
    return static_cast<Derived &>(*this);
  }
#endif
};

class Device : public DeviceAccess<Device> {
public:
  Device() = default;
  Device(
    var::StringView path,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(path, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  Device(Device &&a) noexcept { swap(a); }
  Device &operator=(Device &&a) noexcept {
    swap(a);
    return *this;
  }
};

template <int VersionRequest> class DeviceType {
public:
  static int get_version(const Device &device) {
    return device.ioctl(VersionRequest, nullptr).return_value();
  }
};

} // namespace hal

#endif /* HALAPI_HAL_DEVICE_HPP_ */

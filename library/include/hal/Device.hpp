/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.

#ifndef HALAPI_HAL_DEVICE_HPP_
#define HALAPI_HAL_DEVICE_HPP_

#include "DeviceSignal.hpp"
#include "fs/Aio.hpp"
#include "fs/File.hpp"

namespace hal {

class DeviceObject : public api::ExecutionContext {
public:
  class Channel {
    API_ACCESS_FUNDAMENTAL(Channel, u32, location, 0);
    API_ACCESS_FUNDAMENTAL(Channel, u32, value, 0);
  };

  DeviceObject() {}

  DeviceObject(var::StringView path,
               fs::OpenMode open_mode = fs::OpenMode::read_write()
                   FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST);

  DeviceObject(DeviceObject &&a) { std::swap(m_file, a.m_file); }
  DeviceObject &operator=(DeviceObject &&a) {
    std::swap(m_file, a.m_file);
    return *this;
  }

#if !defined __link

  class Transfer {
    API_AC(Transfer, var::View, source);
    API_AC(Transfer, var::View, destination);
  };

protected:
  void set_interrupt_priority(int priority,
                              int request = I_MCU_SETACTION) const;

  void set_signal_action(const DeviceSignal &signal,
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

  fs::File m_file;

private:
};

template <class Derived>
class DeviceAccess : public DeviceObject, public fs::FileMemberAccess<Derived> {
public:
  DeviceAccess() : fs::FileMemberAccess<Derived>(m_file) {}

  DeviceAccess(const var::StringView path,
               fs::OpenMode open_mode = fs::OpenMode::read_write()
                   FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceObject(path, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST),
        fs::FileMemberAccess<Derived>(m_file) {}

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
    return static_cast<const Derived &>(*this);
  }

  using fs::FileMemberAccess<Derived>::read;
  using fs::FileMemberAccess<Derived>::write;

  const Derived &read(fs::Aio &aio) const {
    DeviceObject::read(aio);
    return static_cast<const Derived &>(*this);
  }

  Derived &read(fs::Aio &aio) {
    DeviceObject::read(aio);
    return static_cast<const Derived &>(*this);
  }

  const Derived &write(fs::Aio &aio) const {
    DeviceObject::write(aio);
    return static_cast<const Derived &>(*this);
  }

  Derived &write(fs::Aio &aio) {
    DeviceObject::write(aio);
    return static_cast<const Derived &>(*this);
  }

  const Derived &transfer(const Transfer &options) const {
    DeviceObject::transfer(options);
    return static_cast<const Derived &>(*this);
  }

  Derived &transfer(const Transfer &options) {
    DeviceObject::transfer(options);
    return static_cast<const Derived &>(*this);
  }
#endif
};

class Device : public DeviceAccess<Device> {
public:
  Device() {}

  Device(var::StringView path,
         fs::OpenMode open_mode = fs::OpenMode::read_write()
             FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess(path, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}
};

template <int VersionRequest> class DeviceType {
public:
  static int get_version(const Device &device) {
    return device.ioctl(VersionRequest, nullptr).return_value();
  }
};

} // namespace hal

#endif /* HALAPI_HAL_DEVICE_HPP_ */

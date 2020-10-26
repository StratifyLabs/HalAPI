/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.

#ifndef HALAPI_HAL_DEVICE_HPP_
#define HALAPI_HAL_DEVICE_HPP_

#include "DeviceSignal.hpp"
#include "fs/Aio.hpp"
#include "fs/File.hpp"

namespace hal {

class Device : public api::ExecutionContext {
public:
  class Channel {
    API_ACCESS_FUNDAMENTAL(Channel, u32, location, 0);
    API_ACCESS_FUNDAMENTAL(Channel, u32, value, 0);
  };

  using Write = fs::File::Write;
  using Whence = fs::File::Whence;
  using Ioctl = fs::File::Ioctl;

  Device() {}

  Device(var::StringView path,
         fs::OpenMode open_mode = fs::OpenMode::read_write()
             FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST);

  const Device &set_interrupt_priority(int priority,
                                       int request = I_MCU_SETACTION) const;

  const Device &read(void *buf, size_t size) const {
    m_file.read(buf, size);
    return *this;
  }

  Device &read(void *buf, size_t size) {
    m_file.read(buf, size);
    return *this;
  }

  const Device &read(var::View view) const {
    m_file.read(view);
    return *this;
  }

  Device &read(var::View view) {
    m_file.read(view);
    return *this;
  }

  const Device &write(const void *buf, size_t size) const {
    m_file.write(buf, size);
    return *this;
  }

  Device &write(const void *buf, size_t size) {
    m_file.write(buf, size);
    return *this;
  }

  const Device &write(var::View view) const {
    m_file.write(view);
    return *this;
  }

  Device &write(var::View view) {
    m_file.write(view);
    return *this;
  }

  const Device &write(const fs::FileObject &source_file,
                      const Write &options = Write()) const {
    m_file.write(source_file, options);
    return *this;
  }

  Device &write(const fs::FileObject &source_file,
                const Write &options = Write()) {
    m_file.write(source_file, options);
    return *this;
  }

  const Device &write(const fs::FileObject &source_file,
                      const var::Transformer &transformer,
                      const Write &options = Write()) const {
    m_file.write(source_file, transformer, options);
    return *this;
  }

  Device &write(const fs::FileObject &source_file,
                const var::Transformer &transformer,
                const Write &options = Write()) {
    m_file.write(source_file, transformer, options);
    return *this;
  }

  const Device &seek(int location, Whence whence = Whence::set) const {
    m_file.seek(location, whence);
    return *this;
  }

  Device &seek(int location, fs::File::Whence whence = Whence::set) {
    m_file.seek(location, whence);
    return *this;
  }

  const Device &ioctl(int request, void *arg) const {
    ioctl(request, arg);
    return *this;
  }

  Device &ioctl(int request, void *arg) {
    ioctl(request, arg);
    return *this;
  }

  const Device &ioctl(const fs::File::Ioctl &options) const {
    ioctl(options);
    return *this;
  }

  Device &ioctl(const fs::File::Ioctl &options) {
    ioctl(options);
    return *this;
  }

  Device &&move() { return std::move(*this); }

#if !defined __link

  const Device &set_signal_action(
    const DeviceSignal &signal,
    const DeviceSignal::CreateAction &options) {
    mcu_action_t action = signal.create_action(options);
    return ioctl(I_MCU_SETACTION, &action);
  }

  const Device &read(fs::Aio &aio) const;
  const Device &write(fs::Aio &aio) const;

  using fs::FileAccess<Device>::read;
  using fs::FileAccess<Device>::write;

  const Device &cancel_read(int channel = 0) const;
  const Device &cancel_write(int channel = 0) const;
  const Device &cancel(int channel, int o_events) const;

  class Transfer {
    API_AC(Transfer, var::View, source);
    API_AC(Transfer, var::View, destination);
  };

  const Device &transfer(const Transfer &options) const;

#endif

protected:
private:
  fs::File m_file;
};

template <int VersionRequest> class DeviceType {
public:
  static int get_version(const Device &device) {
    return device.ioctl(VersionRequest, nullptr).return_value();
  }
};

} // namespace hal

#endif /* HALAPI_HAL_DEVICE_HPP_ */

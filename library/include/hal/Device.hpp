// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_DEVICE_HPP_
#define HALAPI_HAL_DEVICE_HPP_

#include <sos/Link.hpp>

#include "DeviceSignal.hpp"
#include "fs/Aio.hpp"
#include "fs/File.hpp"

namespace hal {

class DeviceObject : public api::ExecutionContext {
public:
#if defined __link
#define DEVICE_OPEN_MODE (fs::OpenMode::read_write().set_non_blocking())
  using DeviceFile = sos::Link::File;
#else
#define DEVICE_OPEN_MODE fs::OpenMode::read_write()
  using DeviceFile = fs::File;
#endif

  class Channel {
    API_ACCESS_FUNDAMENTAL(Channel, u32, location, 0);
    API_ACCESS_FUNDAMENTAL(Channel, u32, value, 0);
  };

#if !defined __link

  class Transfer {
    API_AC(Transfer, var::View, source);
    API_AC(Transfer, var::View, destination);
  };

protected:
  static void set_interrupt_priority_implementation(
    const DeviceFile &file,
    int priority,
    int request = I_MCU_SETACTION);
  static void read_implementation(const DeviceFile &file, fs::Aio &aio);
  static void write_implementation(const DeviceFile &file, fs::Aio &aio);

  static void
  cancel_read_implementation(const DeviceFile &file, int channel = 0);
  static void
  cancel_write_implementation(const DeviceFile &file, int channel = 0);
  static void
  cancel_implementation(const DeviceFile &file, int channel, int o_events);

  static void
  transfer_implementation(const DeviceFile &file, const Transfer &options);

  static void set_signal_action_implementation(
    const DeviceFile &file,
    const DeviceSignal &signal,
    const DeviceSignal::CreateAction &options);

#endif
};

template <class Derived>
class DeviceAccess
  : public DeviceObject,
    public fs::FileMemberAccess<Derived, DeviceObject::DeviceFile> {
  using FileBase = fs::FileMemberAccess<Derived, DeviceObject::DeviceFile>;

public:
  DeviceAccess() = default;

  explicit DeviceAccess(
    const var::StringView path,
    fs::OpenMode open_mode = fs::OpenMode::read_write()
#if defined __link
                               .set_non_blocking()
#endif
                                 FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : fs::FileMemberAccess<Derived, DeviceFile>(
      path,
      open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {
  }

  using FileBase::file;
  using FileBase::ioctl;
  using FileBase::read;
  using FileBase::seek;
  using FileBase::write;

  API_NO_DISCARD int fileno() const { return file().fileno(); }
  API_NO_DISCARD bool is_valid() const { return file().is_valid(); }

#if !defined __link
#define HALAPI_DEVICE_FUNCTION_GROUP(QUAL)                                     \
  auto QUAL cancel(int channel, int o_events) QUAL {                           \
    cancel_implementation(file(), channel, o_events);                          \
    return static_cast<Derived QUAL>(*this);                                   \
  }
  HALAPI_DEVICE_FUNCTION_GROUP(const &)
  HALAPI_DEVICE_FUNCTION_GROUP(&)
  HALAPI_DEVICE_FUNCTION_GROUP(&&)
#undef HALAPI_DEVICE_FUNCTION_GROUP

#define HALAPI_DEVICE_FUNCTION_GROUP(QUAL)                                     \
  auto QUAL cancel_read(int channel = 0) QUAL {                                \
    cancel_read_implementation(file(), channel);                               \
    return static_cast<Derived QUAL>(*this);                                   \
  }
  HALAPI_DEVICE_FUNCTION_GROUP(const &)
  HALAPI_DEVICE_FUNCTION_GROUP(&)
  HALAPI_DEVICE_FUNCTION_GROUP(&&)
#undef HALAPI_DEVICE_FUNCTION_GROUP

#define HALAPI_DEVICE_FUNCTION_GROUP(QUAL)                                     \
  auto QUAL cancel_write(int channel = 0) QUAL {                               \
    cancel_write_implementation(file(), channel);                              \
    return static_cast<Derived QUAL>(*this);                                   \
  }
  HALAPI_DEVICE_FUNCTION_GROUP(const &)
  HALAPI_DEVICE_FUNCTION_GROUP(&)
  HALAPI_DEVICE_FUNCTION_GROUP(&&)
#undef HALAPI_DEVICE_FUNCTION_GROUP

#define HALAPI_DEVICE_FUNCTION_GROUP(QUAL)                                     \
  auto QUAL set_interrupt_priority(int priority) QUAL {                        \
    set_interrupt_priority_implementation(file(), priority);                   \
    return static_cast<Derived QUAL>(*this);                                   \
  }
  HALAPI_DEVICE_FUNCTION_GROUP(const &)
  HALAPI_DEVICE_FUNCTION_GROUP(&)
  HALAPI_DEVICE_FUNCTION_GROUP(&&)
#undef HALAPI_DEVICE_FUNCTION_GROUP

#define HALAPI_DEVICE_FUNCTION_GROUP(QUAL)                                     \
  auto QUAL set_signal_action(                                                 \
    const DeviceSignal &signal,                                                \
    const DeviceSignal::CreateAction &options) QUAL {                          \
    set_signal_action_implementation(file(), signal, options);                 \
    return static_cast<Derived QUAL>(*this);                                   \
  }
  HALAPI_DEVICE_FUNCTION_GROUP(const &)
  HALAPI_DEVICE_FUNCTION_GROUP(&)
  HALAPI_DEVICE_FUNCTION_GROUP(&&)
#undef HALAPI_DEVICE_FUNCTION_GROUP

#define HALAPI_DEVICE_FUNCTION_GROUP(QUAL)                                     \
  auto QUAL read(fs::Aio &aio) QUAL {                                          \
    read_implementation(FileBase::file(), aio);                                \
    return static_cast<Derived QUAL>(*this);                                   \
  }
  HALAPI_DEVICE_FUNCTION_GROUP(const &)
  HALAPI_DEVICE_FUNCTION_GROUP(&)
  HALAPI_DEVICE_FUNCTION_GROUP(&&)
#undef HALAPI_DEVICE_FUNCTION_GROUP

#define HALAPI_DEVICE_FUNCTION_GROUP(QUAL)                                     \
  auto QUAL write(fs::Aio &aio) QUAL {                                         \
    write_implementation(FileBase::file(), aio);                               \
    return static_cast<Derived QUAL>(*this);                                   \
  }
  HALAPI_DEVICE_FUNCTION_GROUP(const &)
  HALAPI_DEVICE_FUNCTION_GROUP(&)
  HALAPI_DEVICE_FUNCTION_GROUP(&&)
#undef HALAPI_DEVICE_FUNCTION_GROUP

#define HALAPI_DEVICE_FUNCTION_GROUP(QUAL)                                     \
  auto QUAL transfer(const Transfer &options) QUAL {                           \
    transfer_implementation(FileBase::file(), options);                        \
    return static_cast<Derived QUAL>(*this);                                   \
  }
  HALAPI_DEVICE_FUNCTION_GROUP(const &)
  HALAPI_DEVICE_FUNCTION_GROUP(&)
  HALAPI_DEVICE_FUNCTION_GROUP(&&)
#undef HALAPI_DEVICE_FUNCTION_GROUP
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
};

template <int VersionRequest> class DeviceType {
public:
  static int get_version(const Device &device) {
    return device.ioctl(VersionRequest).return_value();
  }
};

} // namespace hal

#endif /* HALAPI_HAL_DEVICE_HPP_ */

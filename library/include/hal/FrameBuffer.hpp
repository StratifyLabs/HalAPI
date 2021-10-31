// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_FFIFO_HPP_
#define HALAPI_HAL_FFIFO_HPP_

#include <sos/dev/ffifo.h>

#include "Device.hpp"

namespace hal {

class FrameBuffer : public DeviceAccess<FrameBuffer> {
public:
  enum class Flags {
    set_writeblock = FFIFO_FLAG_SET_WRITEBLOCK,
    is_overflow = FFIFO_FLAG_IS_OVERFLOW,
    is_notify_write = FFIFO_FLAG_NOTIFY_WRITE,
    is_notify_read = FFIFO_FLAG_NOTIFY_READ
  };

  class Info {
  public:
    Info() = default;
    explicit Info(const ffifo_info_t &info) : m_info{info} {}
    API_NO_DISCARD bool is_valid() const { return m_info.frame_size != 0; }
    API_NO_DISCARD u16 frame_count() const { return m_info.frame_count; }
    API_NO_DISCARD u16 frame_size() const { return m_info.frame_size; }
    API_NO_DISCARD u16 frame_count_ready() const {
      return m_info.frame_count_ready;
    }
    API_NO_DISCARD u32 size() const { return frame_size() * frame_count(); }

    API_NO_DISCARD Flags flags() const { return Flags(m_info.o_flags); }

    API_NO_DISCARD u32 o_flags() const { return m_info.o_flags; }

    API_NO_DISCARD bool is_overflow() const {
      return (m_info.o_flags & FFIFO_FLAG_IS_OVERFLOW) != 0;
    }

  private:
    friend class FrameBuffer;
    ffifo_info_t m_info{};
  };

  class Attributes {
  public:
    Attributes() = default;
    explicit Attributes(const ffifo_attr_t &attr) { m_attributes = attr; }

    Attributes &set_overflow() {
      m_attributes.o_flags |= FFIFO_FLAG_IS_OVERFLOW;
      return *this;
    }
    Attributes &set_writeblock() {
      m_attributes.o_flags |= FFIFO_FLAG_SET_WRITEBLOCK;
      return *this;
    }

    ffifo_attr_t *attributes() { return &m_attributes; }
    API_NO_DISCARD const ffifo_attr_t *attributes() const {
      return &m_attributes;
    }

    API_NO_DISCARD Flags flags() const { return Flags(m_attributes.o_flags); }

  private:
    friend class FrameBuffer;
    ffifo_attr_t m_attributes{};
  };

  explicit FrameBuffer(
    const var::StringView device,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  FrameBuffer() = default;
  FrameBuffer(const FrameBuffer &a) = delete;
  FrameBuffer &operator=(const FrameBuffer &a) = delete;

  FrameBuffer(FrameBuffer &&a) noexcept { swap(a); }
  FrameBuffer &operator=(FrameBuffer &&a) noexcept {
    swap(a);
    return *this;
  }

  const FrameBuffer &set_attributes(Attributes &attr) const {
    return ioctl(I_FFIFO_SETATTR, &attr.m_attributes);
  }

  const FrameBuffer &flush() const { return ioctl(I_FFIFO_FLUSH, nullptr); }

  API_NO_DISCARD Info get_info() const {
    ffifo_info_t info;
    ioctl(I_FFIFO_GETINFO, &info);
    return Info(info);
  }
};

API_OR_NAMED_FLAGS_OPERATOR(FrameBuffer, Flags)

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::FrameBuffer::Attributes &a);
Printer &operator<<(Printer &printer, const hal::FrameBuffer::Info &a);
} // namespace printer

#endif // HALAPI_HAL_FFIFO_HPP_

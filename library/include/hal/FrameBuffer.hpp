// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_FFIFO_HPP_
#define HALAPI_HAL_FFIFO_HPP_

#include <sos/dev/ffifo.h>

#include "Device.hpp"

namespace hal {

class FrameBuffer : public DeviceAccess<FrameBuffer> {
public:
  class Info {
  public:
    Info() { m_info = {0}; }
    Info(const ffifo_info_t &info) { m_info = info; }
    bool is_valid() const { return m_info.frame_size != 0; }
    u16 frame_count() const { return m_info.frame_count; }
    u16 frame_size() const { return m_info.frame_size; }
    u16 frame_count_ready() const { return m_info.frame_count_ready; }
    u32 size() const { return frame_size() * frame_count(); }
    bool is_overflow() const {
      return (m_info.o_flags & FFIFO_FLAG_IS_OVERFLOW) != 0;
    }

  private:
    friend class FrameBuffer;
    ffifo_info_t m_info;
  };

  class Attributes {
  public:
    Attributes() { m_attributes = {0}; }

    Attributes(const ffifo_attr_t &attr) { m_attributes = attr; }

    Attributes &set_overflow() {
      m_attributes.o_flags |= FFIFO_FLAG_IS_OVERFLOW;
      return *this;
    }
    Attributes &set_write_block() {
      m_attributes.o_flags |= FFIFO_FLAG_SET_WRITEBLOCK;
      return *this;
    }

    ffifo_attr_t *attributes() { return &m_attributes; }
    const ffifo_attr_t *attributes() const { return &m_attributes; }

  private:
    friend class FrameBuffer;
    ffifo_attr_t m_attributes;
  };

  FrameBuffer(const var::StringView device,
              fs::OpenMode open_mode =
                  DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  FrameBuffer() {}
  FrameBuffer(const FrameBuffer &a) = delete;
  FrameBuffer &operator=(const FrameBuffer &a) = delete;

  FrameBuffer(FrameBuffer &&a) { swap(a); }
  FrameBuffer &operator=(FrameBuffer &&a) {
    swap(a);
    return *this;
  }

  const FrameBuffer &set_attributes(Attributes &attr) const {
    return ioctl(I_FFIFO_SETATTR, &attr.m_attributes);
  }

  const FrameBuffer &flush() const { return ioctl(I_FFIFO_FLUSH, nullptr); }

  Info get_info() const {
    ffifo_info_t info;
    ioctl(I_FFIFO_GETINFO, &info);
    return Info(info);
  }
};

} // namespace hal

#endif // HALAPI_HAL_FFIFO_HPP_

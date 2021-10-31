// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_BYTE_BUFFER_HPP_
#define HALAPI_HAL_BYTE_BUFFER_HPP_

#include <sos/dev/fifo.h>

#include "Device.hpp"

namespace hal {

class ByteBuffer;

class ByteBufferFlags {
public:
  enum class Flags {
    set_writeblock = FIFO_FLAG_SET_WRITEBLOCK,
    is_overflow = FIFO_FLAG_IS_OVERFLOW,
    is_notify_write = FIFO_FLAG_NOTIFY_WRITE,
    is_notify_read = FIFO_FLAG_NOTIFY_READ,
    initialize = FIFO_FLAG_INIT,
    exit = FIFO_FLAG_EXIT,
    flush = FIFO_FLAG_FLUSH
  };
};

API_OR_NAMED_FLAGS_OPERATOR(ByteBufferFlags, Flags)

class ByteBuffer : public DeviceAccess<ByteBuffer>, public ByteBufferFlags {
public:
  /*! \brief Fifo Attributes Class
   *
   */
  class Info {
  public:
    Info() = default;

    Info(const fifo_info_t &info) : m_info(info) {}

    API_NO_DISCARD bool is_valid() const { return m_info.size > 0; }
    API_NO_DISCARD u32 size_ready() const { return m_info.size_ready; }
    API_NO_DISCARD u32 size() const { return m_info.size; }

    API_NO_DISCARD bool is_overflow() const { return m_info.overflow != 0; }
    API_NO_DISCARD bool overflow() const { return m_info.overflow != 0; }

  private:
    friend class ByteBuffer;
    fifo_info_t m_info{};
  };

  class Attributes {
  public:
    Attributes() = default;
    Attributes(const fifo_attr_t &attr) : m_attributes(attr) {}

    Attributes &set_initialize() {
      m_attributes.o_flags |= FIFO_FLAG_INIT;
      return *this;
    }
    Attributes &set_finalize() {
      m_attributes.o_flags |= FIFO_FLAG_EXIT;
      return *this;
    }
    Attributes &set_overflow() {
      m_attributes.o_flags |= FIFO_FLAG_IS_OVERFLOW;
      return *this;
    }
    Attributes &set_writeblock() {
      m_attributes.o_flags |= FIFO_FLAG_SET_WRITEBLOCK;
      return *this;
    }

    API_NO_DISCARD Flags flags() const {
      return static_cast<Flags>(m_attributes.o_flags);
    }

    fifo_attr_t *attributes() { return &m_attributes; }
    API_NO_DISCARD const fifo_attr_t *attributes() const {
      return &m_attributes;
    }

  private:
    friend class ByteBuffer;
    fifo_attr_t m_attributes{};
  };

  ByteBuffer(
    const var::StringView device,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  ByteBuffer() {}
  ByteBuffer(const ByteBuffer &a) = delete;
  ByteBuffer &operator=(const ByteBuffer &a) = delete;

  ByteBuffer(ByteBuffer &&a) noexcept { swap(a); }
  ByteBuffer &operator=(ByteBuffer &&a) noexcept {
    swap(a);
    return *this;
  }

  const ByteBuffer &set_attributes(Attributes &attr) const {
    return ioctl(I_FIFO_SETATTR, &attr.m_attributes);
  }

  ByteBuffer &set_attributes(Attributes &attr) {
    return API_CONST_CAST_SELF(ByteBuffer, set_attributes, attr);
  }

  API_NO_DISCARD Info get_info() const {
    fifo_info_t info;
    ioctl(I_FIFO_GETINFO, &info);
    return Info(info);
  }
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::ByteBuffer::Attributes &a);
Printer &operator<<(Printer &printer, const hal::ByteBuffer::Info &a);
} // namespace printer

#endif /* HALAPI_HAL_BYTE_BUFFER_HPP_ */

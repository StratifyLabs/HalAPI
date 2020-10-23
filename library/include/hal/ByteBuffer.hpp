/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.

#ifndef HALAPI_HAL_BYTE_BUFFER_HPP_
#define HALAPI_HAL_BYTE_BUFFER_HPP_

#include "Device.hpp"
#include <cstring>
#include <sos/dev/fifo.h>

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

class ByteBuffer : public hal::Device, public ByteBufferFlags {
public:
  /*! \brief Fifo Attributes Class
   *
   */
  class Info {
  public:
    Info() { m_info = {0}; }

    Info(const fifo_info_t &info) : m_info(info) {}

    bool is_valid() const { return m_info.size > 0; }
    u32 size_ready() const { return m_info.size_ready; }
    u32 size() const { return m_info.size; }

    bool is_overflow() const { return m_info.overflow != 0; }
    bool overflow() const { return m_info.overflow != 0; }

  private:
    friend class ByteBuffer;
    fifo_info_t m_info;
  };

  class Attributes {
  public:
    Attributes() { m_attributes = {0}; }

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
      m_attributes.o_flags |= FIFO_FLAG_SET_WRITEBLOCK;
      return *this;
    }
    Attributes &set_writeblock() {
      m_attributes.o_flags |= FIFO_FLAG_IS_OVERFLOW;
      return *this;
    }

    Flags flags() const { return static_cast<Flags>(m_attributes.o_flags); }

  private:
    friend class ByteBuffer;
    fifo_attr_t m_attributes;
  };

  ByteBuffer();

  static Device::Ioctl set_attributes(Attributes &attr) {
    return Device::Ioctl()
        .set_request(I_FIFO_SETATTR)
        .set_argument(&attr.m_attributes);
  }

  static Device::Ioctl get_info(Info &info) {
    return Device::Ioctl()
        .set_request(I_FIFO_GETINFO)
        .set_argument(&info.m_info);
  }

  static Info get_info(const Device &device) {
    Info info;
    device.ioctl(get_info(info));
    return info;
  }
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::ByteBuffer::Attributes &a);
Printer &operator<<(Printer &printer, const hal::ByteBuffer::Info &a);
} // namespace printer

#endif /* HALAPI_HAL_BYTE_BUFFER_HPP_ */

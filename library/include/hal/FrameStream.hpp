// Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md for
// rights.
#ifndef HALAPI_HAL_STREAMFFIFO_HPP_
#define HALAPI_HAL_STREAMFFIFO_HPP_

#include <api/api.hpp>

#include <sos/dev/stream_ffifo.h>

#include "FrameBuffer.hpp"

namespace hal {

class StreamFFifo;

class FrameStreamFlags {
public:
  enum class Flags {
    start = STREAM_FFIFO_FLAG_START,
    stop = STREAM_FFIFO_FLAG_STOP,
    flush = STREAM_FFIFO_FLAG_FLUSH
  };
};

API_OR_NAMED_FLAGS_OPERATOR(FrameStreamFlags, Flags)

class FrameStream : public DeviceAccess<FrameStream>, public FrameStreamFlags {
public:
  class ChannelInfo {
  public:
    ChannelInfo(const stream_ffifo_channel_info_t &info) : m_info(info) {}
    bool is_valid() const { return m_info.ffifo.frame_count != 0; }

    FrameBuffer::Info frame_buffer_info() const { return m_info.ffifo; }

    u32 access_count() const { return m_info.access_count; }
    s32 error() const { return m_info.error; }

  private:
    const stream_ffifo_channel_info_t &m_info;
  };

  class Info {
  public:
    Info() : m_receive(m_info.rx), m_transmit(m_info.tx) { m_info = {0}; }

    Info(const stream_ffifo_info_t &info)
        : m_receive(m_info.rx), m_transmit(m_info.tx) {
      m_info = info;
    }

    operator stream_ffifo_info_t &() { return m_info; }
    const stream_ffifo_info_t &info() const { return m_info; }

    bool is_valid() const {
      return receive().is_valid() || transmit().is_valid();
    }

    bool is_stopped() const {
      return (m_info.o_status & STREAM_FFIFO_FLAG_STOP) == 0;
    }

    bool is_running() const {
      return (m_info.o_status & STREAM_FFIFO_FLAG_START) == 0;
    }

    bool is_reset() const { return m_info.o_status == 0; }

    const ChannelInfo &receive() const { return m_receive; }
    const ChannelInfo &transmit() const { return m_transmit; }

  private:
    friend class FrameStream;
    stream_ffifo_info_t m_info;
    ChannelInfo m_receive;
    ChannelInfo m_transmit;
  };

  class Attributes {
  public:
    Attributes() { m_attributes = {0}; }

    operator const stream_ffifo_attr_t &() const { return m_attributes; }

    Attributes &set_flags(Flags value) {
      m_attributes.o_flags = static_cast<u32>(value);
      return *this;
    }

    Attributes &set_start() {
      m_attributes.o_flags |= STREAM_FFIFO_FLAG_START;
      return *this;
    }
    Attributes &set_stop() {
      m_attributes.o_flags |= STREAM_FFIFO_FLAG_STOP;
      return *this;
    }
    Attributes &set_flush() {
      m_attributes.o_flags |= STREAM_FFIFO_FLAG_FLUSH;
      return *this;
    }

  private:
    friend class FrameStream;
    stream_ffifo_attr_t m_attributes;
  };
  FrameStream(const var::StringView device,
              fs::OpenMode open_mode =
                  DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  FrameStream() {}
  FrameStream(const FrameStream &a) = delete;
  FrameStream &operator=(const FrameStream &a) = delete;

  FrameStream(FrameStream &&a) { swap(a); }
  FrameStream &operator=(FrameStream &&a) {
    swap(a);
    return *this;
  }

  const FrameStream &start() const {
    return set_attributes(Attributes().set_start());
  }
  const FrameStream &stop() const {
    return set_attributes(Attributes().set_stop());
  }
  const FrameStream &flush() const {
    return set_attributes(Attributes().set_flush());
  }

  const FrameStream &set_attributes(const Attributes &attr) const {
    return ioctl(I_STREAM_FFIFO_SETATTR, (void *)&attr.m_attributes);
  }

  Info get_info() const {
    stream_ffifo_info_t info;
    ioctl(I_STREAM_FFIFO_GETINFO, &info);
    return Info(info);
  }
};

} // namespace hal

#endif // HALAPI_HAL_STREAMFFIFO_HPP_

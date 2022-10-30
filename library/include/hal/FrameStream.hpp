// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_FRAME_STREAM_HPP_
#define HALAPI_HAL_FRAME_STREAM_HPP_

#include <api/api.hpp>

#include <sos/dev/stream_ffifo.h>

#include "FrameBuffer.hpp"

namespace hal {

class FrameStream : public DeviceAccess<FrameStream> {
public:
  enum class Flags {
    start = STREAM_FFIFO_FLAG_START,
    stop = STREAM_FFIFO_FLAG_STOP,
    flush = STREAM_FFIFO_FLAG_FLUSH
  };

  class ChannelInfo {
  public:
    explicit ChannelInfo(const stream_ffifo_channel_info_t &info)
      : m_info(&info) {}
    API_NO_DISCARD bool is_valid() const {
      return m_info->ffifo.frame_count != 0;
    }
    API_NO_DISCARD FrameBuffer::Info frame_buffer_info() const {
      return FrameBuffer::Info(m_info->ffifo);
    }
    API_NO_DISCARD u32 access_count() const { return m_info->access_count; }
    API_NO_DISCARD s32 error() const { return m_info->error; }

  private:
    const stream_ffifo_channel_info_t *m_info = nullptr;
  };

  class Info {
  public:
    Info() : m_receive(m_info.rx), m_transmit(m_info.tx) {}

    Info(const stream_ffifo_info_t &info)
      : m_info(info), m_receive(m_info.rx), m_transmit(m_info.tx) {}

    operator stream_ffifo_info_t &() { return m_info; }
    API_NO_DISCARD const stream_ffifo_info_t &info() const { return m_info; }

    API_NO_DISCARD bool is_valid() const {
      return receive().is_valid() || transmit().is_valid();
    }

    API_NO_DISCARD bool is_stopped() const {
      return (m_info.o_status & STREAM_FFIFO_FLAG_STOP) == 0;
    }

    API_NO_DISCARD bool is_running() const {
      return (m_info.o_status & STREAM_FFIFO_FLAG_START) == 0;
    }

    API_NO_DISCARD bool is_reset() const { return m_info.o_status == 0; }

    API_NO_DISCARD const ChannelInfo &receive() const { return m_receive; }
    API_NO_DISCARD const ChannelInfo &transmit() const { return m_transmit; }

    API_NO_DISCARD Flags flags() const { return Flags(m_info.o_flags); }

  private:
    friend class FrameStream;
    stream_ffifo_info_t m_info{};
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

    stream_ffifo_attr_t *attributes() { return &m_attributes; }
    API_NO_DISCARD const stream_ffifo_attr_t *attributes() const {
      return &m_attributes;
    }

    API_NO_DISCARD Flags flags() const { return Flags(m_attributes.o_flags); }

  private:
    friend class FrameStream;
    stream_ffifo_attr_t m_attributes;
  };
  explicit FrameStream(
    const var::StringView device,
    fs::OpenMode open_mode
    = DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
    : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  FrameStream() = default;

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

  API_NO_DISCARD Info get_info() const {
    stream_ffifo_info_t info;
    ioctl(I_STREAM_FFIFO_GETINFO, &info);
    return {info};
  }
};

API_OR_NAMED_FLAGS_OPERATOR(FrameStream, Flags)

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::FrameStream::Attributes &a);
Printer &operator<<(Printer &printer, const hal::FrameStream::Info &a);
Printer &operator<<(Printer &printer, const hal::FrameStream::ChannelInfo &a);
} // namespace printer

#endif // HALAPI_HAL_FRAME_STREAM_HPP_

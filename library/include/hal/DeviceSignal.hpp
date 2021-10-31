// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_DEVICESIGNAL_HPP_
#define HALAPI_HAL_DEVICESIGNAL_HPP_

#if !defined __link

#include <sos/fs/devfs.h>

#include <thread/Signal.hpp>
#include <thread/Thread.hpp>

namespace hal {

class DeviceSignal : public thread::Signal {
public:
  enum class IsPersistent { no, yes };

  DeviceSignal(
    IsPersistent persistent,
    thread::Signal::Number signo,
    int sigvalue = 0)
    : thread::Signal(signo, sigvalue), m_context{
                                         .tid = int(thread::Thread::self()),
                                         .si_signo = static_cast<int>(signo),
                                         .si_sigcode = SI_USER,
                                         .sig_value = sigvalue,
                                         .keep
                                         = persistent == IsPersistent::yes} {}

  DeviceSignal(
    IsPersistent persistent,
    thread::Signal::Number signo,
    void *sigvalue = nullptr)
    : thread::Signal(signo, sigvalue), m_context{
                                         .tid = int(thread::Thread::self()),
                                         .si_signo = int(signo),
                                         .si_sigcode = SI_USER,
                                         .sig_ptr = sigvalue,
                                         .keep
                                         = persistent == IsPersistent::yes} {}

  explicit DeviceSignal(const devfs_signal_callback_t &context)
    : thread::Signal(
      static_cast<thread::Signal::Number>(context.si_signo),
      context.sig_value) {
    this->m_context = context;
  }

  class CreateAction {
    API_AF(CreateAction, u32, o_events, 0);
    API_AF(CreateAction, u8, channel, 0);
    API_AF(CreateAction, u32, priority, 0);
  };

  API_NO_DISCARD mcu_action_t create_action(const CreateAction &options) const {
    return {
      .channel = options.channel(),
      .prio = s8(options.priority()),
      .o_events = options.o_events(),
      .handler
      = {.callback = devfs_signal_callback, .context = (void *)&m_context}};
  }

private:
  devfs_signal_callback_t m_context{};
};

} /* namespace hal */

#endif

#endif /* HALAPI_HAL_DEVICESIGNAL_HPP_ */

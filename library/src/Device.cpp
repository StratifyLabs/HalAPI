/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.
// Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc

#include "hal/Device.hpp"

using namespace hal;

#ifndef __link
void DeviceObject::read(fs::Aio &aio) const {
  API_RETURN_IF_ERROR();
  aio.m_aio_var.aio_fildes = m_file.fileno();
  API_SYSTEM_CALL("", ::aio_read(&(aio.m_aio_var)));
}

void DeviceObject::write(fs::Aio &aio) const {
  API_RETURN_IF_ERROR();
  aio.m_aio_var.aio_fildes = m_file.fileno();
  API_SYSTEM_CALL("", ::aio_write(&(aio.m_aio_var)));
}

void DeviceObject::cancel_read(int channel) const {
  return cancel(channel, MCU_EVENT_FLAG_DATA_READY);
}

void DeviceObject::cancel_write(int channel) const {
  cancel(channel, MCU_EVENT_FLAG_WRITE_COMPLETE);
}

void DeviceObject::cancel(int channel, int o_events) const {
  mcu_action_t action;
  memset(&action, 0, sizeof(action));
  action.channel = channel;
  action.o_events = o_events;
  m_file.ioctl(I_MCU_SETACTION, &action);
}

void DeviceObject::transfer(const Transfer &options) const {
  fs::Aio aio(options.destination());
  read(aio);
  m_file.write(options.source());
  while (aio.is_busy() && is_success()) {
    // aio must live until the read completes -- or big problems will happen
    chrono::wait(chrono::MicroTime(200));
  }
}


void DeviceObject::set_interrupt_priority(int priority, int request) const {
  mcu_action_t action;
  memset(&action, 0, sizeof(action));
  action.o_events = MCU_EVENT_FLAG_SET_PRIORITY;
  action.prio = priority;
  m_file.ioctl(request, &action);
}
#endif

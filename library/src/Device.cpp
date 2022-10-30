// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include "hal/Device.hpp"

using namespace hal;
using namespace chrono;

#ifndef __link
void DeviceObject::read_implementation(const DeviceFile &file, fs::Aio &aio) {
  API_RETURN_IF_ERROR();
  aio.m_aio_var.aio_fildes = file.fileno();
  API_SYSTEM_CALL("", ::aio_read(&(aio.m_aio_var)));
}

void DeviceObject::write_implementation(const DeviceFile &file, fs::Aio &aio) {
  API_RETURN_IF_ERROR();
  aio.m_aio_var.aio_fildes = file.fileno();
  API_SYSTEM_CALL("", ::aio_write(&(aio.m_aio_var)));
}

void DeviceObject::cancel_read_implementation(
  const DeviceFile &file,
  int channel) {
  return cancel_implementation(file, channel, MCU_EVENT_FLAG_DATA_READY);
}

void DeviceObject::cancel_write_implementation(
  const DeviceFile &file,
  int channel) {
  cancel_implementation(file, channel, MCU_EVENT_FLAG_WRITE_COMPLETE);
}

void DeviceObject::cancel_implementation(
  const DeviceFile &file,
  int channel,
  int o_events) {
  auto action = mcu_action_t{.channel = u8(channel), .o_events = u8(o_events)};
  file.ioctl(I_MCU_SETACTION, &action);
}

void DeviceObject::transfer_implementation(
  const DeviceFile &file,
  const Transfer &options) {
  fs::Aio aio(options.destination());
  read_implementation(file, aio);
  file.write(options.source());
  while (aio.is_busy() && is_success()) {
    // aio must live until the read completes -- or big problems will happen
    wait(200_microseconds);
  }
}

void DeviceObject::set_interrupt_priority_implementation(
  const DeviceFile &file,
  int priority,
  int request) {
  auto action = mcu_action_t{
    .prio = s8(priority),
    .o_events = MCU_EVENT_FLAG_SET_PRIORITY};
  file.ioctl(request, &action);
}

void DeviceObject::set_signal_action_implementation(
  const DeviceObject::DeviceFile &file,
  const DeviceSignal &signal,
  const DeviceSignal::CreateAction &options) {
  auto action = signal.create_action(options);
  file.ioctl(I_MCU_SETACTION, &action);
}
#endif

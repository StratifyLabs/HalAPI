// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#ifndef HALAPI_HAL_I2C_HPP_
#define HALAPI_HAL_I2C_HPP_

#include <sos/dev/i2c.h>

#include "Device.hpp"

namespace hal {

class I2CFlags {
public:
  enum class Flags {
    null = 0,
    set_master = I2C_FLAG_SET_MASTER,
    set_slave = I2C_FLAG_SET_SLAVE,
    is_slave_ack_general_call = I2C_FLAG_IS_SLAVE_ACK_GENERAL_CALL,
    is_pullup = I2C_FLAG_IS_PULLUP,
    prepare_ptr_data = I2C_FLAG_PREPARE_PTR_DATA,
    is_ptr_16 = I2C_FLAG_IS_PTR_16,
    prepare_ptr = I2C_FLAG_PREPARE_PTR,
    prepare_data = I2C_FLAG_PREPARE_DATA,
    is_slave_addr0 = I2C_FLAG_IS_SLAVE_ADDR0,
    is_slave_addr1 = I2C_FLAG_IS_SLAVE_ADDR1,
    is_slave_addr2 = I2C_FLAG_IS_SLAVE_ADDR2,
    is_slave_addr3 = I2C_FLAG_IS_SLAVE_ADDR3,
    is_slave_ptr_8 = I2C_FLAG_IS_SLAVE_PTR_8,
    is_slave_ptr_16 = I2C_FLAG_IS_SLAVE_PTR_16,
    reset = I2C_FLAG_RESET,
    is_stretch_clock = I2C_FLAG_STRETCH_CLOCK,
    is_no_stop = I2C_FLAG_IS_NO_STOP
  };

  enum class Error {
    none = I2C_ERROR_NONE,
    start = I2C_ERROR_START,
    write = I2C_ERROR_WRITE,
    ack = I2C_ERROR_ACK,
    stop = I2C_ERROR_STOP,
    master_ack = I2C_ERROR_MASTER_ACK,
    bus_busy = I2C_ERROR_BUS_BUSY,
    long_slew = I2C_ERROR_LONG_SLEW,
    arbitration_lost = I2C_ERROR_ARBITRATION_LOST
  };
};

API_OR_NAMED_FLAGS_OPERATOR(I2CFlags, Flags)

class I2C : public DeviceAccess<I2C>, public I2CFlags {
public:
  class Attributes {
  public:
    Attributes() {
      set_frequency(100000);
      var::View(m_attributes.pin_assignment).fill<u8>(0xff);
    }

    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      sda)

    API_ACCESS_MEMBER_FUNDAMENTAL(
      Attributes,
      mcu_pin_t,
      attributes.pin_assignment,
      scl)

    /*! \details Access the slave address value. */
    u8 slave_addr() const { return m_attributes.slave_addr[0].addr8[0]; }

    /*! \details Accesses the 16-bit slave address value. */
    u8 slave_addr16() const { return m_attributes.slave_addr[0].addr16; }

    Attributes &set_flags(Flags value) {
      m_attributes.o_flags = static_cast<u32>(value);
      return *this;
    }

    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(
      Attributes,
      u32,
      attributes,
      frequency,
      freq)

    /*! \details Sets the 7-bit slave address value. */
    Attributes &set_slave_addr(u8 addr) {
      m_attributes.slave_addr[0].addr8[0] = addr;
      return *this;
    }

    /*! \details Sets the 16-bit slave address value. */
    Attributes &set_slave_addr16(u16 addr) {
      m_attributes.slave_addr[0].addr16 = addr;
      return *this;
    }

    i2c_attr_t *attributes() { return &m_attributes; }
    const i2c_attr_t *attributes() const { return &m_attributes; }

  private:
    friend class I2C;
    i2c_attr_t m_attributes;
  };

  class Info {
  public:
    Info() { m_info = {0}; }
    Info(const i2c_info_t &info) : m_info(info) {}
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(Info, u32, info, frequency, freq)
    API_READ_ACCESS_MEMBER_FUNDAMENTAL(Info, u32, info, o_events)
    API_ACCESS_MEMBER_FUNDAMENTAL_WITH_ALIAS(Info, u32, info, error, err)
    Flags o_flags() const { return static_cast<Flags>(m_info.o_flags); }

  private:
    friend class I2C;
    i2c_info_t m_info;
  };

  I2C(const var::StringView device,
      fs::OpenMode open_mode =
          DEVICE_OPEN_MODE FSAPI_LINK_DECLARE_DRIVER_NULLPTR_LAST)
      : DeviceAccess(device, open_mode FSAPI_LINK_INHERIT_DRIVER_LAST) {}

  I2C() {}
  I2C(const I2C &a) = delete;
  I2C &operator=(const I2C &a) = delete;

  I2C(I2C &&a) { swap(a); }
  I2C &operator=(I2C &&a) {
    swap(a);
    return *this;
  }

  const I2C &set_attributes() const {
    return ioctl(I_I2C_SETATTR, nullptr);
  }

  I2C &set_attributes() {
    return API_CONST_CAST_SELF(I2C, set_attributes);
  }

  using ScanResult = var::Array<u8, 128>;

  ScanResult scan() const;

  const I2C &set_attributes(const Attributes &attributes) const {
    return ioctl(I_I2C_SETATTR, (void *)&attributes.m_attributes);
  }

  I2C &set_attributes(const Attributes &attributes) {
    return API_CONST_CAST_SELF(I2C, set_attributes, attributes);
  }

  const I2C &prepare(u8 slave_addr, Flags o_flags = Flags::prepare_ptr_data) const {
    Attributes attributes
      = Attributes().set_flags(o_flags).set_slave_addr(slave_addr);
    return ioctl(I_I2C_SETATTR, &attributes.m_attributes);
  }

  I2C &prepare(u8 slave_addr, Flags o_flags = Flags::prepare_ptr_data) {
    return API_CONST_CAST_SELF(I2C, prepare, slave_addr, o_flags);
  }

  const I2C &reset() const {
    Attributes attributes = Attributes().set_flags(Flags::reset);
    return ioctl(I_I2C_SETATTR, &attributes.m_attributes);
  }

  I2C &reset() {
    return API_CONST_CAST_SELF(I2C, reset);
  }

  int get_error() { return get_info().error(); }

  Info get_info() {
    i2c_info_t info{};
    ioctl(I_I2C_GETINFO, &info);
    return Info(info);
  }

  class ModifyRegister {
    API_AF(ModifyRegister, u32, location, 0);
    API_AF(ModifyRegister, u8, bit, 0);
    API_AF(ModifyRegister, bool, value, false);
  };

  I2C &modify_register(const ModifyRegister &options) {
    u8 value;
    seek(options.location()).read(var::View(value));
    if (options.value()) {
      value |= (1 << options.bit());
    } else {
      value &= (1 << options.bit());
    }
    return seek(options.location()).write(var::View(value));
  }

  I2C &operator()(const ModifyRegister &options) {
    return modify_register(options);
  }

private:
};

} // namespace hal

namespace printer {
Printer &operator<<(Printer &printer, const hal::I2C::Attributes &a);
Printer &operator<<(Printer &printer, const hal::I2C::Info &a);
} // namespace printer

#endif /* HALAPI_HAL_I2C_HPP_ */

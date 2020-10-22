/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md for rights.

#ifndef HALAPI_HAL_HPP_
#define HALAPI_HAL_HPP_

namespace hal {}

#include "hal/Device.hpp"
#include "hal/I2C.hpp"
#include "hal/Spi.hpp"
#include "hal/Uart.hpp"

#if !defined __link
#include "hal/DisplayDevice.hpp"
#include "hal/DeviceSignal.hpp"
#endif

using namespace hal;

#endif /* HALAPI_HAL_HPP_ */

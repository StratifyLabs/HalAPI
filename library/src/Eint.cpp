/*! \file */ // Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc; see
             // LICENSE.md for rights.
// Copyright 2011-2020 Tyler Gilbert and Stratify Labs, Inc

#include "hal/Eint.hpp"
#include <fcntl.h>
#include <unistd.h>
using namespace hal;

Eint::Eint(port_t port) : Periph(CORE_PERIPH_EINT, port) {}

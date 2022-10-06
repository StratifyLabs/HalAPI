# Version 1.3.0

## New Features

- Port to `CMakeSDK` v2.0

# Version 1.2.0

## New Features

- Update `CMakeLists.txt` to require setup of SDK in a super-project
- add `.clang-format` file
- Add printer operator for `I2C::ScanResult`
- Add `Pin::PulseScope` for using RAII to set and reset a pin

## Bug Fixes

- Add `writeBlockCount` when printing `hal::Drive::Info`
- Reformat and Tidy

# Version 1.1.0

## New Features

- Add `Flash` class for accessing flash devices
- Add additional methods to `Drive` class for easier access to attribute flags

## Bug Fixes

- None

# Version 1.0.0

Initial stable release.

# HalAPI

HalAPI is a hardware abstraction access API following the Stratify
Labs [API framework](https://github.com/StratifyLabs/API). This can be used on desktop (windows, macOS, linux) using a
target device that is running Stratify OS. But the most common usage is running the HalAPI in Stratify OS applications
on Cortex M microcontrollers.

## Building

The HalAPI is designed to be built as part of an SDK super project. Instructions for building are at
the [SDK API project](https://github.com/StratifyLabs/SdkAPI).

## Usage

The HalAPI is a simple wrapper library for POSIX style file system access of Stratify OS device drivers. Check out the
UART example:

```c
#include <unistd.h>
#include <fcntl.h>
#include <sos/dev/uart.h>

//using just C and unistd
int fd = open("/dev/uart", O_RDWR);
uart_attr_t attr = {0};
attr.freq = 115200;
attr.o_flags = UART_FLAG_SET_DEFAULT_LINE_CODING;
ioctl(fd, I_UART_SETATTR, &attr);
const char str[] = "Hello World\n";
write(fd, str, sizeof(str));
close(fd);

//using HalAPI and C++
//the code below condenses the code above

#include <hal.hpp>

Uart("/dev/uart")
  .set_attributes(Uart::Attributes().set_frequency(115200))
  .write("Hello World\n");
```

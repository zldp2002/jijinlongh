

***
# XFEL
Tiny FEL tools for allwinner SOC, support RISC-V D1 chip.

[Examples](https://xboot.github.io/xfel) &mdash;
[Documentation](https://xboot.github.io/xfel) &mdash;
[Release](https://github.com/xboot/xfel/releases/)

## How to build

### Linux platform

The xfel tools depends on the `libusb-1.0` library, you need to install `libusb-1.0-0-dev` before compile, for example in ubuntu:

```shell
sudo apt install libusb-1.0-0-dev
```

Then just type `make` at the root directory, you will see a binary program.

```shell
cd xfel
make
sudo make install
```

### Window platform

Install some build tools

```shell
sudo apt install mingw-w64
sudo apt install autoconf
sudo apt install libtool-bin
```


```shell
git clone https://github.com/libusb/libusb.git
cd libusb
./autogen.sh
./configure --host=i686-w64-mingw32 --prefix=/usr/i686-w64-mingw32/
make
sudo make install
```

Build xfel source code

```shell
cd xfel
CROSS=i686-w64-mingw32- make
```

For 64-bits windows, you can using `x86_64-w64-mingw32-` instead of `i686-w64-mingw32` above.


## Usage

```shell
xfel(v1.2.3) - https://github.com/xboot/xfel
usage:
    xfel version                                        - Show chip version
    xfel hexdump <address> <length>                     - Dumps memory region in hex
    xfel dump <address> <length>                        - Binary memory dump to stdout
    xfel exec <address>                                 - Call function address
    xfel read32 <address>                               - Read 32-bits value from device memory
    xfel write32 <address> <value>                      - Write 32-bits value to device memory
    xfel read <address> <length> <file>                 - Read memory to file
    xfel write <address> <file>                         - Write file to memory
    xfel reset                                          - Reset device using watchdog
    xfel sid                                            - Show sid information
    xfel jtag                                           - Enable jtag debug
    xfel ddr [type]                                     - Initial ddr controller with optional type
    xfel spinor                                         - Detect spi nor flash
    xfel spinor read <address> <length> <file>          - Read spi nor flash to file
    xfel spinor write <address> <file>                  - Write file to spi nor flash
    xfel spinand                                        - Detect spi nand flash
    xfel spinand read <address> <length> <file>         - Read spi nand flash to file
    xfel spinand write <address> <file>                 - Write file to spi nand flash
    xfel spinand splwrite <split-size> <address> <file> - Write file to spi nand flash with split support
```

## Links

* [The chinese discussion posts](https://whycan.com/t_6546.html)
* [The  linux-sunxi community](http://sunxi.org/)

## License

This library is free software; you can redistribute it and or modify it under the terms of the MIT license. See [MIT License](LICENSE) for details.


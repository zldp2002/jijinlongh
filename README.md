

***
# XFEL
Tiny FEL tools for allwinner SOC, support RISC-V D1 chip.

## How to build

The xfel tools depends on the `libusb-1.0` library, you need to install `libusb-1.0-0-dev` before compile, for example in ubuntu:
```shell
sudo apt install libusb-1.0-0-dev
```

The just type `make` at the root directory, you will see a binary program.

```shell
cd xfel
make
```

## Usage

```
xfel(v1.0.0) - https://github.com/xboot/xfel
usage:
    xfel help                                   - Print this usage
    xfel version                                - Show brom version
    xfel hexdump <address> <length>             - Dumps memory region in hex
    xfel dump <address> <length>                - Binary memory dump to stdout
    xfel exec <address>                         - Call function address
    xfel read32 <address>                       - Read 32-bits value from device memory
    xfel write32 <address> <value>              - Write 32-bits value to device memory
    xfel read <address> <length> <file>         - Read memory to file
    xfel write <address> <file>                 - Write file to memory
    xfel reset                                  - Reset device using watchdog
    xfel sid                                    - Show 128-bits SID information
    xfel jtag                                   - Enable JTAG debug
    xfel ddr [type]                             - Initial DDR controller with optional type
    xfel spinor                                 - Detect spi nor flash
    xfel spinor read <address> <length> <file>  - Read spi nor flash to file
    xfel spinor write <address> <file>          - Write file to spi nor flash
    xfel spinand                                - Detect spi nand flash
    xfel spinand read <address> <length> <file> - Read spi nand flash to file
    xfel spinand write <address> <file>         - Write file to spi nand flash
```

## Links

* [The chinese discussion posts](https://whycan.com/t_6546.html)
* [The  linux-sunxi community](http://sunxi.org/)

## License

This library is free software; you can redistribute it and or modify it under the terms of the MIT license. See [MIT License](LICENSE) for details.




***
# XFEL
Tiny FEL tools for allwinner SOC.

[Documentation](https://xboot.github.io/xfel) &mdash;
[Support Lists](https://xboot.github.io/xfel#/?id=support-lists) &mdash;
[Examples](https://xboot.github.io/xfel#/?id=examples) &mdash;
[Releases](https://github.com/xboot/xfel/releases/)

## Usage

```shell
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
    xfel spinor erase <address> <length>                - Erase spi nor flash
    xfel spinor read <address> <length> <file>          - Read spi nor flash to file
    xfel spinor write <address> <file>                  - Write file to spi nor flash
    xfel spinand                                        - Detect spi nand flash
    xfel spinand erase <address> <length>               - Erase spi nand flash
    xfel spinand read <address> <length> <file>         - Read spi nand flash to file
    xfel spinand write <address> <file>                 - Write file to spi nand flash
    xfel spinand splwrite <split-size> <address> <file> - Write file to spi nand flash with split support
```

## Links

* [The chinese discussion posts](https://whycan.com/t_6546.html)
* [The linux-sunxi community](http://sunxi.org/)

## License

This software is free software; you can redistribute it and or modify it under the terms of the MIT license. See [MIT License](LICENSE) for details.


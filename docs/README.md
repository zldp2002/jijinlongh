<p align="center"><img width="300" src="" alt="XFEL logo"></p>
<h1 align="center">XFEL</h1>
<p align="center">Tiny FEL tools for Allwinner SOC.</p>

## About XFEL

Tiny FEL tools for Allwinner SOC.

## FEL
FEL is a low-level subroutine contained in the BootROM on Allwinner devices. It is used for initial programming and recovery of devices using USB.

## Supported Platfroms

| Chip |  Code  |  Status  |
| -------- | -----: | :----: |
| A10 | $1      |   5    |
| A10s | $1      |   6    |
| A13      | $1      |   7    |

## USAGE

```
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

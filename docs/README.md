<p align="center"><img width="300" src="https://avatars.githubusercontent.com/u/423027?s=200&v=4" alt="XFEL logo"></p>
<h1 align="center">XFEL</h1>
<p align="center">Tiny FEL tools for Allwinner SOC.</p>

## About XFEL

Tiny FEL tools for Allwinner SOC.

## FEL
FEL is a low-level subroutine contained in the BootROM on Allwinner devices. It is used for initial programming and recovery of devices using USB.

## Support Lists

✅: Supported — ❌: Not Supported Yet  — ⚠️: Not Fully Supported Yet

| Chip |  CPU  |  ID  |  Basic  |  Reset  | Sid | Jtag | DDR | Spi Nor Flash | Spi Nand Flash |
| -------- | :----- | :----- | -------- | :------- | -------- | -------- | -------- | -------- | -------- |
| A10 | ARM Cortex-A8 @ 1Ghz | 0x00162300 |   ✅   | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| A10s | ARM Cortex-A8 @ 1Ghz | 0x00162500 |   ✅   | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| A13 | ARM Cortex-A8 @ 1Ghz | 0x00162500 |   ✅   | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| R8 | ARM Cortex-A8 @ 1Ghz | 0x00162500 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| A20 | Dual-Core ARM Cortex-A7 | 0x00165100 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| A23 | Dual-Core ARM Cortex-A7 @ 1.5GHz | 0x00165000 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| A31 | Quad-Core ARM Cortex-A7 | 0x00163300 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| A33 | Quad-Core ARM Cortex-A7 @ 1.2GHz | 0x00166700 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| R16 | Quad-Core ARM Cortex-A7 @ 1.2GHz | 0x00166700 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| A40i | Quad-Core ARM Cortex-A7 @ 1.2 GHz | 0x00170100 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| R40 | Quad-Core ARM Cortex-A7 @ 1.2 GHz | 0x00170100 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| A64 | Quad-Core ARM Cortex-A53 | 0x00168900 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| A80 | Octa-core big.LITTLE Cortex-A15/7 | 0x00163900 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| A83T | Octa-core Cortex-A7 @ 1.6GHz | 0x00167300 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| D1 | XuanTie C906 RISC-V | 0x00185900 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| D1s | XuanTie C906 RISC-V | 0x00185900 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| F133 | XuanTie C906 RISC-V | 0x00185900 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| F1C100S | Single-core ARM9 | 0x00166300 | ✅ | ✅ | ⚠️ | ✅ | ✅ | ✅ | ✅ |
| F1C200S | Single-core ARM9 | 0x00166300 | ✅ | ✅ | ⚠️ | ✅ | ✅ | ✅ | ✅ |
| F1C500S | Single-core ARM9 | 0x00166300 | ✅ | ✅ | ⚠️ | ✅ | ✅ | ✅ | ✅ |
| H2 | Quad-Core ARM Cortex-A7 | 0x00168000 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| H3 | Quad-Core ARM Cortex-A7 @ 1.296GHz | 0x00168000 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| H5 | Quad-Core ARM Cortex-A53 | 0x00171800 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| H6 | Quad-Core ARM Cortex-A53 @ 1.8GHz | 0x00172800 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| H616 | Quad-Core ARM Cortex-A53 | 0x00182300 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| R328 | Dual-Core Cortex-A7 | 0x00182100 | ✅ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ |
| R329 | Dual-Core Cortex-A53 | 0x00185100 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| R528 | Dual-Core Cortex-A7 | 0x00185900 | ✅ | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ |
| T507 | Quad-Core ARM Cortex-A53 | 0x00182300 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| V3s | Single-core Cortex-A7 | 0x00168100 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| S3 | Single-core Cortex-A7 | 0x00168100 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| V536 | Dual-Core Cortex-A7 | 0x00181600 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| V831 | Single-core Cortex-A7 800Mhz | 0x00181700 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |

## Usage

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

## Examples

#### F1C100s: Write U-Boot to DDR and execute U-Boot

```
xfel ddr                               # Initial ddr controller
xfel write 0x81700000 u-boot.bin       # write uboot to 0x81700000
xfel exec 0x81700000                   # Call the function and exectue
```

#### F133: Write OpenSBI to DDR and execute it


```
xfel ddr ddr2                          # Initial ddr controller with ddr2 type
xfel write 0x80200000 opensbi.bin      # write opensbi to 0x80200000
xfel exec 0x80200000                   # Call the function and exectue
```


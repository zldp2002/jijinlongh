<p align="center"><img width="300" src="https://avatars.githubusercontent.com/u/423027?s=200&v=4" alt="XFEL logo"></p>
<h1 align="center">XFEL</h1>
<p align="center">Tiny FEL tools for Allwinner SOC.</p>

## About XFEL

Tiny FEL tools for Allwinner SOC.

## FEL
FEL is a low-level subroutine contained in the BootROM on Allwinner devices. It is used for initial programming and recovery of devices using USB.

## Support List

✅: Supported — ❌: Not Supported Yet  — ⚠️: Not Fully Supported Yet

| Chip |  CPU  |  ID  |  Status  |  Note  |
| -------- | :----- | :----- | -------- | :------- |
| Allwinner A10 | ARM Cortex-A8 @ 1Ghz | 0x00162300 |   ❌   |      |
| Allwinner A10s | ARM Cortex-A8 @ 1Ghz | 0x00162500 |   ❌   |      |
| Allwinner A13 | ARM Cortex-A8 @ 1Ghz | 0x00162500 |   ❌   |      |
| Allwinner R8 | ARM Cortex-A8 @ 1Ghz | 0x00162500 | ❌ |  |
| Allwinner A20 | Dual-Core ARM Cortex-A7 | 0x00165100 | ❌ |  |
| Allwinner A23 | Dual-Core ARM Cortex-A7 @ 1.5GHz | 0x00165000 | ❌ |  |
| Allwinner A31 | Quad-Core ARM Cortex-A7 | 0x00163300 | ❌ |  |
| Allwinner A33 | Quad-Core ARM Cortex-A7 @ 1.2GHz | 0x00166700 | ❌ |  |
| Allwinner R16 | Quad-Core ARM Cortex-A7 @ 1.2GHz | 0x00166700 | ❌ |  |
| Allwinner A40i | Quad-Core ARM Cortex-A7 @ 1.2 GHz | 0x00170100 | ❌ |  |
| Allwinner R40 | Quad-Core ARM Cortex-A7 @ 1.2 GHz | 0x00170100 | ❌ |  |
| Allwinner A64 | Quad-Core ARM Cortex-A53 | 0x00168900 | ❌ |  |
| Allwinner A80 | Octa-core big.LITTLE Cortex-A15/7 | 0x00163900 | ❌ |  |
| Allwinner A83T | Octa-core Cortex-A7 @ 1.6GHz | 0x00167300 | ❌ |  |
| Allwinner D1 | XuanTie C906 RISC-V | 0x00185900 | ✅ |  |
| Allwinner F133 (D1s) | XuanTie C906 RISC-V | 0x00185900 | ✅ |  |
| Allwinner F1C100S | Single-core ARM9 | 0x00166300 | ✅ |  |
| Allwinner F1C200S | Single-core ARM9 | 0x00166300 | ✅ |  |
| Allwinner F1C500S | Single-core ARM9 | 0x00166300 | ✅ |  |
| Allwinner H2 | Quad-Core ARM Cortex-A7 | 0x00168000 | ✅ |  |
| Allwinner H3 | Quad-Core ARM Cortex-A7 @ 1.296GHz | 0x00168000 | ✅ |  |
| Allwinner H5 | Quad-Core ARM Cortex-A53 | 0x00171800 | ❌ |  |
| Allwinner H6 | Quad-Core ARM Cortex-A53 @ 1.8GHz | 0x00172800 | ❌ |  |
| Allwinner H616 | Quad-Core ARM Cortex-A53 | 0x00182300 | ❌ |  |
| Allwinner R328 | Dual-Core Cortex-A7 | 0x00182100 | ⚠️ | Only chip_sid |
| Allwinner R329 | Dual-Core Cortex-A53 | 0x00185100 | ❌ |  |
| Allwinner T507 | Quad-Core ARM Cortex-A53 | 0x00182300 | ❌ |  |
| Allwinner V3s | Single-core Cortex-A7 | 0x00168100 | ✅ |  |
| Allwinner S3 | Single-core Cortex-A7 | 0x00168100 | ✅ |  |
| Allwinner V536 | Dual-Core Cortex-A7 | 0x00181600 | ❌ |  |
| Allwinner V831 | Single-core Cortex-A7 800Mhz | 0x00181700 | ✅ |  |

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

## EXAMPLE

#### F1C100s: Write U-Boot to DDR and execute U-Boot

```
xfel ddr ddr2                          # Initial ddr controller with ddr2 type
xfel write 0x81700000 u-boot.bin       # write uboot to 0x81700000
xfel exec 0x81700000                   # Call the function and exectue
```

#### F133: Write OpenSBI to DDR and execute it


```
xfel ddr ddr2                          # Initial ddr controller with ddr2 type
xfel write 0x80200000 opensbi.bin      # write opensbi to 0x80200000
xfel exec 0x80200000                   # Call the function and exectue
```


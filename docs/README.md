<p align="center"><img width="300" src="https://avatars.githubusercontent.com/u/423027?s=200&v=4" alt="XFEL logo"></p>
<h1 align="center">XFEL</h1>
<p align="center">Tiny FEL tools for Allwinner SOC.</p>

# About XFEL

Tiny FEL tools for Allwinner SOC.

## FEL
FEL is a low-level subroutine contained in the BootROM on Allwinner devices. It is used for initial programming and recovery of devices using USB.

# Usage

```
usage:
    xfel version                                        - Show chip version
    xfel hexdump <address> <length>                     - Dumps memory region in hex
    xfel dump <address> <length>                        - Binary memory dump to stdout
    xfel read32 <address>                               - Read 32-bits value from device memory
    xfel write32 <address> <value>                      - Write 32-bits value to device memory
    xfel read <address> <length> <file>                 - Read memory to file
    xfel write <address> <file>                         - Write file to memory
    xfel exec <address>                                 - Call function address
    xfel reset                                          - Reset device using watchdog
    xfel sid                                            - Show sid information
    xfel jtag                                           - Enable jtag debug
    xfel ddr [type]                                     - Initial ddr controller with optional type
    xfel sign <public-key> <private-key> <file>         - Generate ecdsa256 signature file for sha256 of sid
    xfel spinor                                         - Detect spi nor flash
    xfel spinor erase <address> <length>                - Erase spi nor flash
    xfel spinor read <address> <length> <file>          - Read spi nor flash to file
    xfel spinor write <address> <file>                  - Write file to spi nor flash
    xfel spinand                                        - Detect spi nand flash
    xfel spinand erase <address> <length>               - Erase spi nand flash
    xfel spinand read <address> <length> <file>         - Read spi nand flash to file
    xfel spinand write <address> <file>                 - Write file to spi nand flash
    xfel spinand splwrite <split-size> <address> <file> - Write file to spi nand flash with split support
    xfel extra [...]                                    - The extra commands
```

# Support Lists

✅: Supported — ❌: Not Supported Yet  — ⚠️: Not Fully Supported Yet

| Chip |  CPU  |  ID  |  Basic  |  Reset  | Sid | Jtag | DDR | SPI Nor Flash | SPI Nand Flash |
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
| A40i | Quad-Core ARM Cortex-A7 @ 1.2 GHz | 0x00170100 | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ❌ |
| R40 | Quad-Core ARM Cortex-A7 @ 1.2 GHz | 0x00170100 | ✅ | ✅ | ✅ | ❌ | ✅ | ❌ | ❌ |
| A50 | Quad-Core ARM Cortex-A7 @ 1.8GHz | 0x00175500 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
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
| R128 | ARM Cortex-M33 and C906 RISC-V | 0x00188300 | ✅ | ✅ | ✅ | ✅ | ✅ | ❌ | ❌ |
| R328 | Dual-Core Cortex-A7 | 0x00182100 | ✅ | ❌ | ✅ | ❌ | ❌ | ❌ | ❌ |
| R329 | Dual-Core Cortex-A53 | 0x00185100 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| R528 | Dual-Core Cortex-A7 | 0x00185900 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| T113 | Dual-Core Cortex-A7 | 0x00185900 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| T507 | Quad-Core ARM Cortex-A53 | 0x00182300 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| V3s | Single-core Cortex-A7 | 0x00168100 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| S3 | Single-core Cortex-A7 | 0x00168100 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| V536 | Dual-Core Cortex-A7 | 0x00181600 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| V831 | Single-core Cortex-A7 800Mhz | 0x00181700 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| V853 | Single-core Cortex-A7 1Ghz + E907 RISC-V | 0x00188600 | ✅ | ❌ | ❌ | ❌ | ✅ | ❌ | ❌ |

# Build from source

## Linux platform

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

## Window platform

Windows adopts the cross-compilation method, to install the cross-compilation tool chain in Ubuntu, using:

```shell
sudo apt install mingw-w64
sudo apt install autoconf
sudo apt install libtool-bin
```
And build libusb for cross-compilation.

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

## macOS platform

?> _TODO_ Add macOS ARM support

Before start, make sure the `Command Line Tools` is installed.

Install `libusb` using [brew](https://brew.sh/)

```shell
brew install libusb
```

Then just type `make` at the root directory, you will see a binary program.

```shell
cd xfel
make
```

# Download and install

## Windows platform

Get the latest prebuild at [RELEASE](https://github.com/xboot/xfel/releases/latest)

# Examples

## F1C100s: Write U-Boot to DDR and execute U-Boot

```
xfel ddr                               # Initial ddr controller
xfel write 0x81700000 u-boot.bin       # write uboot to 0x81700000
xfel exec 0x81700000                   # Call the function and exectue
```

## F1C200s: Write System to SPI NAND Flash
```
xfel spinand
xfel spinand erase 0x000000 134217728
xfel spinand write 0x000000 u-boot-sunxi-with-nand-spl.bin 
xfel spinand write 0x80000 splash.bmp 
xfel spinand write 0x100000 kernel.itb 
xfel spinand write 0x600000 rootfs.squashfs
```

## F133: Write OpenSBI to DDR and execute it

```
xfel ddr f133                          # Initial ddr controller for f133
xfel write 0x80200000 opensbi.bin      # write opensbi to 0x80200000
xfel exec 0x80200000                   # Call the function and exectue
```

?> Want to add more examples? Using the pull requests at [https://github.com/xboot/xfel/pulls](https://github.com/xboot/xfel/pulls)

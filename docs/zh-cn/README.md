<p align="center"><img width="300" src="https://avatars.githubusercontent.com/u/423027?s=200&v=4" alt="XFEL logo"></p>
<h1 align="center">XFEL</h1>
<p align="center">Tiny FEL tools for Allwinner SOC.</p>

# 关于 XFEL

面向全志 SOC 的 FEL 工具。

## 什么是FEL
FEL 是全志 SOC 中上 BootROM 中包含的低级程序。可以通过它使用 USB OTG 对 SOC 进行编程和恢复。

# 支持列表

✅: 已经支持 — ❌: 还未支持  — ⚠️: 还未完全支持

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
| R528 | Dual-Core Cortex-A7 | 0x00185900 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| T507 | Quad-Core ARM Cortex-A53 | 0x00182300 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| V3s | Single-core Cortex-A7 | 0x00168100 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| S3 | Single-core Cortex-A7 | 0x00168100 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| V536 | Dual-Core Cortex-A7 | 0x00181600 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| V831 | Single-core Cortex-A7 800Mhz | 0x00181700 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |

# 下载安装

## Linux

### ArchLinux

xfel 现在可以在 [AUR](https://aur.archlinux.org/packages/xfel/) 下载到, 对于 ArchLinux 只需要使用 AUR 管理器就可以安装。例如`yay`。

```shell
yay -S xfel
```

## Windows

在 [RELEASE](https://github.com/xboot/xfel/releases/latest) 找到预编译的Windows软件。


# 使用方法

```
usage:
    xfel version                                        - 显示芯片ID
    xfel hexdump <address> <length>                     - 以HEX文件转储内存区域
    xfel dump <address> <length>                        - 转储内存区域并输出
    xfel exec <address>                                 - 调用函数地址
    xfel read32 <address>                               - 从设备内存中读取 32 位值
    xfel write32 <address> <value>                      - 向设备内存中写入 32 位值
    xfel read <address> <length> <file>                 - 向文件写入内存
    xfel write <address> <file>                         - 向内存写入文件
    xfel reset                                          - 使用看门狗重置设备
    xfel sid                                            - 显示sid信息
    xfel jtag                                           - 启用 jtag 调试
    xfel ddr [type]                                     - 初始化 DDR 控制器
    xfel spinor                                         - 检测 spi nor flash
    xfel spinor erase <address> <length>                - 擦除 spi nor flash
    xfel spinor read <address> <length> <file>          - 读取 spi nor flash 到文件
    xfel spinor write <address> <file>                  - 将文件写入 spi nor flash
    xfel spinand                                        - 检测 spi nand flash
    xfel spinand erase <address> <length>               - 擦除 spi nand flash
    xfel spinand read <address> <length> <file>         - 读取 spi nand flash 到文件
    xfel spinand write <address> <file>                 - 将文件写入 spi nand flash
    xfel spinand splwrite <split-size> <address> <file> - 使用拆分支持将文件写入 spi nand flash
```

# 编译安装

## Linux

xfel 工具依赖于`libusb-1.0` 库，编译前需要安装`libusb-1.0-0-dev`，在Ubuntu系统中：

```shell
sudo apt install libusb-1.0-0-dev
```

然后在根目录输入`make`与`sudo make install`，就安装完成了。

```shell
cd xfel
make
sudo make install
```

## macOS

?> _TODO_ Add macOS ARM support

在开始之前，请先安装完成`Command Line Tools`.

使用 [brew](https://brew.sh/) 安装`libusb` 依赖。

```shell
brew install libusb
```

然后在程序根目录输入`make`就编译完成了。

```shell
cd xfel
make
```


## Window

Windows 采用交叉编译方法，在 Ubuntu 中安装交叉编译工具链：

```shell
sudo apt install mingw-w64
sudo apt install autoconf
sudo apt install libtool-bin
```
克隆`libusb`源码，编译Windows下的libusb库。

```shell
git clone https://github.com/libusb/libusb.git
cd libusb
./autogen.sh
./configure --host=i686-w64-mingw32 --prefix=/usr/i686-w64-mingw32/
make
sudo make install
```

编译xfel工具

```shell
cd xfel
CROSS=i686-w64-mingw32- make
```

对于交叉编译64位Windows程序，您可以使用 `x86_64-w64-mingw32-` 代替上面的 `i686-w64-mingw32`。

# 例子

## F1C100s: 使用XEFL写入 U-Boot 并运行

```
xfel ddr                               # 初始化DDR控制器
xfel write 0x81700000 u-boot.bin       # 将uboot写入0x81700000
xfel exec 0x81700000                   # 调用函数地址运行
```

## F1C200s: 向 SPI NAND 写入系统
```
xfel spinand
xfel spinand erase 0x000000 134217728
xfel spinand write 0x000000 u-boot-sunxi-with-nand-spl.bin 
xfel spinand write 0x80000 splash.bmp 
xfel spinand write 0x100000 kernel.itb 
xfel spinand write 0x600000 rootfs.squashfs
```


## F133: 使用XEFL写入 OpenSBI 并运行


```
xfel ddr ddr2                          # 使用ddr2方式初始化DDR控制器
xfel write 0x80200000 opensbi.bin      # 将opensbi写入0x80200000
xfel exec 0x80200000                   # 调用函数地址运行
```

?> _TODO_ 还想添加其他例子? 请给我们提交 Pull Requests [https://github.com/xboot/xfel/pulls](https://github.com/xboot/xfel/pulls)

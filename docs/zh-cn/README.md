<p align="center"><img width="300" src="https://avatars.githubusercontent.com/u/423027?s=200&v=4" alt="XFEL logo"></p>
<h1 align="center">XFEL</h1>
<p align="center">Tiny FEL tools for Allwinner SOC.</p>

## 关于 XFEL

面向全志 SOC 的 FEL 工具。

## 什么是FEL
FEL 是全志 SOC 中上 BootROM 中包含的低级程序。可以通过它使用 USB OTG 对 SOC 进行编程和恢复。

## 支持列表

✅: 已经支持 — ❌: 还未支持  — ⚠️: 还未完全支持

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
| R328 | Dual-Core Cortex-A7 | 0x00182100 | ✅ | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ |
| R329 | Dual-Core Cortex-A53 | 0x00185100 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| T507 | Quad-Core ARM Cortex-A53 | 0x00182300 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| V3s | Single-core Cortex-A7 | 0x00168100 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| S3 | Single-core Cortex-A7 | 0x00168100 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| V536 | Dual-Core Cortex-A7 | 0x00181600 | ✅ | ❌ | ❌ | ❌ | ❌ | ❌ | ❌ |
| V831 | Single-core Cortex-A7 800Mhz | 0x00181700 | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |

## 使用方法

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
    xfel spinor read <address> <length> <file>          - 读取 spi nor flash 到文件
    xfel spinor write <address> <file>                  - 将文件写入 spi nor flash
    xfel spinand                                        - 检测 spi nand flash
    xfel spinand read <address> <length> <file>         - 读取 spi nand flash 到文件
    xfel spinand write <address> <file>                 - 将文件写入 spi nand flash
    xfel spinand splwrite <split-size> <address> <file> - 使用拆分支持将文件写入 spi nand flash
```

## 例子

#### F1C100s: 使用XEFL写入 U-Boot 并运行

```
xfel ddr ddr2                          # 使用 ddr2 方式初始化 DDR 控制器
xfel write 0x81700000 u-boot.bin       # 将 uboot 写入 0x81700000
xfel exec 0x81700000                   # 调用函数地址运行
```

#### F133: 使用XEFL写入 OpenSBI 并运行


```
xfel ddr ddr2                          # 使用 ddr2 方式初始化 DDR 控制器
xfel write 0x80200000 opensbi.bin      # 将 opensbi 写入 0x80200000
xfel exec 0x80200000                   # 调用函数地址运行
```

